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

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctime>
#include "StereoDisparity.h"
#include "StereoHelper.h"
#include "Toolkit.h"
#include "SegmentTree.h"

#include <iostream>
using namespace std;


void stereo_routine(const char *left_input, const char *right_input, const char *output, 
					int max_dis_level, int scale, float sigma, METHOD method) {
	cv::Mat leftImg = cv::imread(left_input, CV_LOAD_IMAGE_COLOR );
	cv::Mat rightImg = cv::imread(right_input, CV_LOAD_IMAGE_COLOR );

	cv::Mat dispImg;
	if( !leftImg.data || !rightImg.data ) {
		printf( "Error: can not open image\n" );
		exit( - 1 );
	} else {
		printf( "Image: %s \t %s\n", left_input, right_input );
	}
	if(method == ST_RAW) {
		printf( "Method: ST_RAW\n" );
		stereo_disparity_normal(leftImg, rightImg, dispImg, max_dis_level, scale, sigma);
	} else if(method == ST_REFINED) {
		printf( "Method: ST_REFINED\n" );
		stereo_disparity_iteration(leftImg, rightImg, dispImg, max_dis_level, scale, sigma);
	}

	cv::imwrite(output, dispImg);
}

void stereo_disparity_normal(cv::InputArray left_image, cv::InputArray right_image, cv::OutputArray disp_, 
					  int max_dis_level, int scale, float sigma) {
	cv::Mat imL = left_image.getMat();
	cv::Mat imR = right_image.getMat();
	
	CV_Assert(imL.size() == imR.size());
	CV_Assert(imL.type() == CV_8UC3 && imR.type() == CV_8UC3);

	cv::Size imageSize = imL.size();

	disp_.create(imageSize, CV_8U);
	cv::Mat disp = disp_.getMat();
	
	CDisparityHelper dispHelper;
	
//step 1: cost initialization
	printf( "Cost Init\n" );
	cv::Mat costVol = dispHelper.GetMatchingCost(imL, imR, max_dis_level);

//step 2: cost aggregation
	printf( "Cost Aggr\n" );
	CSegmentTree stree;
	CColorWeight cWeight(imL);
	stree.BuildSegmentTree(imL, sigma, TAU, cWeight);
	stree.Filter(costVol, max_dis_level);

//step 3: disparity computation
	printf( "Disp computation\n" );
	cv::Mat disparity = dispHelper.GetDisparity_WTA((float*)costVol.data, 
		imageSize.width, imageSize.height, max_dis_level);

	MeanFilter(disparity, disparity, 3);

	disparity *= scale;
	disparity.copyTo(disp);
}

void stereo_disparity_iteration(cv::InputArray left_image, cv::InputArray right_image, cv::OutputArray disp_, 
					  int max_dis_level, int scale, float sigma) {
	cv::Mat imL = left_image.getMat();
	cv::Mat imR = right_image.getMat();
	
	CV_Assert(imL.size() == imR.size());
	CV_Assert(imL.type() == CV_8UC3 && imR.type() == CV_8UC3);

	cv::Size imageSize = imL.size();

	disp_.create(imageSize, CV_8U);
	cv::Mat disp = disp_.getMat();
	
	CDisparityHelper dispHelper;

//start of first run
	cv::Mat costVolLeft = dispHelper.GetMatchingCost(imL, imR, max_dis_level);
	cv::Mat costVolRight = dispHelper.GetRightMatchingCostFromLeft(costVolLeft, 
		imageSize.width, imageSize.height, max_dis_level);

	CSegmentTree stree;
	CColorWeight colorLWeight(imL), colorRWeight(imR);

	//left disparity
	printf( "Left Disp\n" );
	stree.BuildSegmentTree(imL, SIGMA_ONE, TAU, colorLWeight);
	stree.Filter(costVolLeft, max_dis_level);
	cv::Mat disparityLeft = dispHelper.GetDisparity_WTA((float *)costVolLeft.data,
		imageSize.width, imageSize.height, max_dis_level);
	MeanFilter(disparityLeft, disparityLeft, 3);

	//right disparity
	printf( "Right Disp\n" );
	stree.BuildSegmentTree(imR, SIGMA_ONE, TAU, colorRWeight);
	stree.Filter(costVolRight, max_dis_level);
	cv::Mat disparityRight =  dispHelper.GetDisparity_WTA((float *)costVolRight.data,
		imageSize.width, imageSize.height, max_dis_level);
	MeanFilter(disparityRight, disparityRight, 3);
	
	//left-right check with right view disparity
	cv::Mat mask(imageSize, CV_8U, cv::Scalar(0));
	cv::Mat occtable(imageSize, CV_8U, cv::Scalar(0));
	
	cv::Mat1b leftPtr = disparityLeft;
	cv::Mat1b rightPtr = disparityRight;
	cv::Mat1b occPtr = occtable;
	cv::Mat1b maskPtr = mask;
	for(int y = 0;y < imageSize.height;y++) {
		for(int x = 0;x < imageSize.width;x++) {
			int d = leftPtr(y, x);
			if(x - d >= 0) {
				int d_cor = rightPtr(y, x - d);
				occPtr(y, x) = (d == 0 || abs(d - d_cor) > 1);
			} else {
				occPtr(y, x) = 1;
			}
			maskPtr(y, x) = !occPtr(y, x);
		}
	}

//re-segmentation and second run	
	printf( "Re-segmentation and second run\n" );
	cv::Mat costVol = dispHelper.GetMatchingCost(imL, imR, max_dis_level);
	CColorDepthWeight colorDepthWeight(imL, disparityLeft, mask, max_dis_level);
	stree.BuildSegmentTree(imL, sigma, TAU, colorDepthWeight);
	stree.Filter(costVol, max_dis_level);
	cv::Mat disparity = dispHelper.GetDisparity_WTA((float *)costVol.data, 
		imageSize.width, imageSize.height, max_dis_level);
	MeanFilter(disparity, disparity, 3);

	disparity *= scale;
	disparity.copyTo(disp);
}


