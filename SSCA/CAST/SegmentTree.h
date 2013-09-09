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

#ifndef __SEGMENT_TREE_H__
#define __SEGMENT_TREE_H__
#include <opencv2/core/core.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#define DEF_CHAR_MAX 255
#define NUM_NEIGHBOR 4
#define MAXNUM_CHILD (NUM_NEIGHBOR - 1)
class CWeightProvider {
public:
	virtual float GetWeight(int x0, int y0, int x1, int y1) const = 0;
	virtual float GetScale() const = 0;
};

struct NodePointer {
	int id;
	uchar dist;

	NodePointer():id(0), dist(0) {}
};

struct TreeNode {
	NodePointer father;
	NodePointer children[NUM_NEIGHBOR];
	int id;
	int childrenNum;

	TreeNode():id(0), childrenNum(0) {}
};

class CSegmentTree {
private:
	float m_table[DEF_CHAR_MAX+1];
	cv::Size m_imgSize;
	std::vector<TreeNode> m_tree;
public:
	void BuildSegmentTree(cv::Mat img, float sigma, float tau, CWeightProvider &weightProvider);
	void UpdateTable(float sigma_range);
	void Filter(cv::Mat costVol, int maxLevel);
};

class CColorWeight: public CWeightProvider {
private:
	cv::Mat img;
	cv::Mat3b imgPtr;
	
public:
	CColorWeight(cv::Mat &img_);

	virtual float GetWeight(int x0, int y0, int x1, int y1) const;
	virtual float GetScale() const {return 1.0f;}
};

class CColorDepthWeight: public CWeightProvider {
private:
	cv::Mat img;
	cv::Mat3b imgPtr;
	cv::Mat1b disp;
	cv::Mat1b mask;
	float level;

public:
	CColorDepthWeight(cv::Mat &img_, cv::Mat &disp_, cv::Mat& mask_, int maxLevel);

	virtual float GetWeight(int x0, int y0, int x1, int y1) const;
	virtual float GetScale() const {return 255.0f;}
};


struct edge
{
	float w;
	int a, b;
	bool operator < (const edge &b) const {
		if(this->w != b.w) {
			return this->w < b.w;
		} else if(this->b != b.b) {
			return this->b < b.b;
		} else {
			return this->a < b.a;
		}
	}
};//graph edge



#endif