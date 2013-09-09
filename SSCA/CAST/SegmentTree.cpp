/*
This project implements the methods in the following paper. Please cite this paper, depending on the use.

Xing Mei, Xun Sun, Weiming Dong, Haitao Wang and Xiaopeng Zhang. Segment-Tree based Cost Aggregation for Stereo Matching, in CVPR 2013.

The code is written by Yan Kong, <kongyanwork@gmail.com>, 2013.

LICENSE
Copyright (C) 2012-2013 by Yan Kong
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "SegmentTree.h"
#include "disjoint-set.h"
#include "segment-graph.h"
#include "Toolkit.h"
#include <algorithm>

#include <iostream>
using namespace std;

void CSegmentTree::BuildSegmentTree(cv::Mat img, float sigma, float tau, CWeightProvider &weightProvider) {
	UpdateTable(sigma);
	m_imgSize = img.size();
	int pixelsNum = m_imgSize.area();

//step 1: build segment tree
	edge *edges = new edge[m_imgSize.area() * NUM_NEIGHBOR / 2];
	int edgeNum = 0;
	for(int y = 0;y < m_imgSize.height;y++) {
		for(int x = 0;x < m_imgSize.width;x++) {
			if(x < m_imgSize.width - 1) {
				edges[edgeNum].a = y * m_imgSize.width + x;
				edges[edgeNum].b = y * m_imgSize.width + (x + 1);
				edges[edgeNum].w = weightProvider.GetWeight(x, y, x + 1, y);
				edgeNum++;
			}

			if(y >= 1) {
				edges[edgeNum].a = y * m_imgSize.width + x;
				edges[edgeNum].b = (y - 1) * m_imgSize.width + x;
				edges[edgeNum].w = weightProvider.GetWeight(x, y, x, y - 1);
				edgeNum++;
			}
		}
	}

	uchar *edges_mask = new uchar[edgeNum];
	memset(edges_mask, 0, sizeof(uchar) * edgeNum);
	universe * u = segment_graph(pixelsNum, edgeNum, edges, tau, edges_mask);

	CV_Assert(1 == u->num_sets());

//step 2: build node based graph
	TreeNode *AdjTable = new TreeNode[pixelsNum];
	for(int i = 0;i < pixelsNum;i++) AdjTable[i].id = i;

	for(int i = 0;i < edgeNum;i++) {
		if(!edges_mask[i]) continue;

		int pa = edges[i].a;
		int pb = edges[i].b;
		int dis = std::min(int(edges[i].w * weightProvider.GetScale() + 0.5f), 255);

		int x0, y0, x1, y1;
		x0 = pa % m_imgSize.width; y0 = pa / m_imgSize.width;
		x1 = pb % m_imgSize.width; y1 = pb / m_imgSize.width;

		TreeNode &nodeA = AdjTable[pa];
		TreeNode &nodeB = AdjTable[pb];
		
		nodeA.children[nodeA.childrenNum].id = pb;
		nodeA.children[nodeA.childrenNum].dist = (uchar)dis; 
		nodeA.childrenNum++;

		nodeB.children[nodeB.childrenNum].id = pa;
		nodeB.children[nodeB.childrenNum].dist = (uchar)dis;
		nodeB.childrenNum++;
	}

//step 3: build ordered tree
	if(!m_tree.empty()) m_tree.clear();
	m_tree.resize(pixelsNum);

	bool *isVisited = new bool[pixelsNum];
	memset(isVisited, 0, sizeof(bool) * pixelsNum);

	m_tree[0] = AdjTable[0];
	isVisited[0] = true;
	int start = 0, end = 1;

	while(start < end) {
		TreeNode &p = m_tree[start++];

		for(int i = 0;i < p.childrenNum;i++) {
			if(isVisited[p.children[i].id]) continue; 

			isVisited[p.children[i].id] = true;

			TreeNode c;
			c.id = p.children[i].id;
			c.father.id = p.id;
			c.father.dist = p.children[i].dist;

			TreeNode &t = AdjTable[c.id];
			for(int j = 0;j < t.childrenNum;j++) {
				if(t.children[j].id != p.id) {
					c.children[c.childrenNum++] = t.children[j];
				}
			}

			m_tree[end++] = c;
		}
	}
	
	CV_Assert(start == pixelsNum && end == pixelsNum);

	delete []isVisited;
	delete []AdjTable;
	delete u;
	delete []edges_mask;
	delete []edges;
}

void CSegmentTree::UpdateTable(float sigma_range) {
	sigma_range = std::max(0.01f, sigma_range);
	for(int i = 0;i <= DEF_CHAR_MAX;i++) {
		m_table[i] = exp(-float(i) / (DEF_CHAR_MAX * sigma_range));
	}
}

void CSegmentTree::Filter(cv::Mat costVol, int maxLevel) {
	cv::Mat costBuffer = costVol.clone();
	
	KIdx_<float, 3>  costPtr((float *)costVol.data, m_imgSize.height, m_imgSize.width, maxLevel);
	KIdx_<float, 3>  bufferPtr((float *)costBuffer.data, m_imgSize.height, m_imgSize.width, maxLevel);


	int pixelsNum = m_imgSize.area();
//first pass: from leaf to root
	for(int i = pixelsNum - 1;i >= 0;i--) {
		TreeNode &node = m_tree[i];
		float *cost = &bufferPtr(node.id * maxLevel);
		for(int z = 0;z < node.childrenNum;z++) {
			float *child_cost = &bufferPtr(node.children[z].id * maxLevel);
			float weight = m_table[node.children[z].dist];
			for(int k = 0;k < maxLevel;k++) {
				cost[k] += child_cost[k] * weight;
			}
		}
	}

//second pass: from root to leaf
	memcpy(&costPtr(0), &bufferPtr(0), sizeof(float) * maxLevel);
	for(int i = 1;i < pixelsNum;i++) {
		TreeNode &node = m_tree[i];
		float *final_cost = &costPtr(node.id * maxLevel);
		float *cur_cost = &bufferPtr(node.id * maxLevel);
		float *father_cost = &costPtr(node.father.id * maxLevel);
		float weight = m_table[node.father.dist];
		for(int k = 0;k < maxLevel;k++) {
			final_cost[k] = weight * (father_cost[k] - weight * cur_cost[k]) + cur_cost[k];
		}
	}
}

CColorWeight::CColorWeight(cv::Mat &img_) {
	img = img_.clone();
	MeanFilter(img, img, 1);
	imgPtr = img;
}

float CColorWeight::GetWeight(int x0, int y0, int x1, int y1) const {
	return (float)std::max(
		std::max( abs(imgPtr(y0,x0)[0] - imgPtr(y1,x1)[0]), abs(imgPtr(y0,x0)[1] - imgPtr(y1,x1)[1]) ), 
		abs(imgPtr(y0,x0)[2] - imgPtr(y1,x1)[2])
		);
}

CColorDepthWeight::CColorDepthWeight(cv::Mat &img_, cv::Mat &disp_, cv::Mat& mask_, int maxLevel):
	disp(disp_), mask(mask_) { 
		img = img_.clone();
		MeanFilter(img, img, 1);
		imgPtr = img;
		level = (float)maxLevel; 
}

float CColorDepthWeight::GetWeight(int x0, int y0, int x1, int y1) const {
#define ALPHA_DEP_SEG 0.5f
	if(mask(y0, x0) && mask(y1, x1)) {
		float dispValue = abs(disp(y0, x0) - disp(y1, x1)) / level;
		float colorValue = std::max(
			std::max( abs(imgPtr(y0,x0)[0] - imgPtr(y1,x1)[0]), abs(imgPtr(y0,x0)[1] - imgPtr(y1,x1)[1]) ), 
			abs(imgPtr(y0,x0)[2] - imgPtr(y1,x1)[2])
			) / 255.0f;
		return (ALPHA_DEP_SEG * dispValue + (1.0f - ALPHA_DEP_SEG) * colorValue);
	} else {
		return (float)std::max(
			std::max( abs(imgPtr(y0,x0)[0] - imgPtr(y1,x1)[0]), abs(imgPtr(y0,x0)[1] - imgPtr(y1,x1)[1]) ), 
			abs(imgPtr(y0,x0)[2] - imgPtr(y1,x1)[2])
			) / 255.0f;
	}
}