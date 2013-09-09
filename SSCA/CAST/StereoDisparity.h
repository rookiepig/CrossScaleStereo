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

#ifndef __STEREO_DISPARITY_H__
#define __STEREO_DISPARITY_H__

#include <opencv2/core/core.hpp>

#ifdef _DEBUG
#pragma comment( lib, "opencv_calib3d246d.lib" )
#pragma comment( lib, "opencv_contrib246d.lib" )
#pragma comment( lib, "opencv_core246d.lib" )
#pragma comment( lib, "opencv_features2d246d.lib" )
#pragma comment( lib, "opencv_flann246d.lib" )
#pragma comment( lib, "opencv_gpu246d.lib" )
#pragma comment( lib, "opencv_highgui246d.lib" )
#pragma comment( lib, "opencv_imgproc246d.lib" )
#pragma comment( lib, "opencv_legacy246d.lib" )
#pragma comment( lib, "opencv_ml246d.lib" )
#pragma comment( lib, "opencv_nonfree246d.lib" )
#pragma comment( lib, "opencv_objdetect246d.lib" )
#pragma comment( lib, "opencv_photo246d.lib" )
#pragma comment( lib, "opencv_stitching246d.lib" )
#pragma comment( lib, "opencv_superres246d.lib" )
#pragma comment( lib, "opencv_ts246d.lib" )
#pragma comment( lib, "opencv_video246d.lib" )
#pragma comment( lib, "opencv_videostab246d.lib" )
#else
#pragma comment( lib, "opencv_calib3d246.lib" )
#pragma comment( lib, "opencv_contrib246.lib" )
#pragma comment( lib, "opencv_core246.lib" )
#pragma comment( lib, "opencv_features2d246.lib" )
#pragma comment( lib, "opencv_flann246.lib" )
#pragma comment( lib, "opencv_gpu246.lib" )
#pragma comment( lib, "opencv_highgui246.lib" )
#pragma comment( lib, "opencv_imgproc246.lib" )
#pragma comment( lib, "opencv_legacy246.lib" )
#pragma comment( lib, "opencv_ml246.lib" )
#pragma comment( lib, "opencv_nonfree246.lib" )
#pragma comment( lib, "opencv_objdetect246.lib" )
#pragma comment( lib, "opencv_photo246.lib" )
#pragma comment( lib, "opencv_stitching246.lib" )
#pragma comment( lib, "opencv_superres246.lib" )
#pragma comment( lib, "opencv_ts246.lib" )
#pragma comment( lib, "opencv_video246.lib" )
#pragma comment( lib, "opencv_videostab246.lib" )
#endif

enum METHOD {ST_RAW, ST_REFINED};
void stereo_routine(const char *left_input, const char *right_input, const char *output, 
					int max_dis_level, int scale, float sigma, METHOD method);

void stereo_disparity_normal(cv::InputArray left_image, cv::InputArray right_image, cv::OutputArray disp, 
					  int max_dis_level, int scale, float sigma);

void stereo_disparity_iteration(cv::InputArray left_image, cv::InputArray right_image, cv::OutputArray disp, 
					  int max_dis_level, int scale, float sigma);

#endif