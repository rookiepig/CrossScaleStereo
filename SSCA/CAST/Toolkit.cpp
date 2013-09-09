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

#include "Toolkit.h"
#include "ctmf.h"
#include <iostream>

void MeanFilter(cv::InputArray iImage_, cv::OutputArray oImage_, int r) {
	cv::Mat iImage = iImage_.getMat();
	cv::Size imageSize = iImage.size();
	CV_Assert(iImage.depth() == CV_8U);

	cv::Mat tmp(imageSize, iImage.type());
	ctmf(iImage.data, tmp.data, imageSize.width, imageSize.height,
		iImage.step1(), tmp.step1(), r, 
		iImage.channels(), imageSize.area() * iImage.channels());

	if(oImage_.getMat().size() != imageSize || oImage_.getMat().depth() != CV_8U || oImage_.getMat().type() != CV_8UC1) {
		oImage_.create(imageSize, iImage.type());
	}

	tmp.copyTo(oImage_.getMat());
}