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

#ifndef __STEREO_HELPER__
#define __STEREO_HELPER__

#include <opencv2/core/core.hpp>
#include <bitset>
using namespace std;
using namespace cv;
// #define USE_CENCUS
#define CENCUS_WND 3
#define CENCUS_BIT 8

class CDisparityHelper {
public:
	cv::Mat GetGradient(cv::InputArray image);
	cv::Mat GetMatchingCost(cv::Mat imL, cv::Mat imR, int maxLevel);
	cv::Mat GetMatchingCost_CEN(cv::Mat imL, cv::Mat imR, int maxLevel);
	cv::Mat GetDisparity_WTA(float *costVol, int w, int h, int maxLevel);
	cv::Mat GetRightMatchingCostFromLeft(cv::Mat leftVol, int w, int h, int maxLevel);
};

#endif