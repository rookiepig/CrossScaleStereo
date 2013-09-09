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
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm> 

#include "StereoHelper.h"
#include "Toolkit.h"

inline unsigned char rgb_2_gray(unsigned char * in) { return((unsigned char)(0.299*in[2]+0.587*in[1]+0.114*in[0]+0.5));}

cv::Mat CDisparityHelper::GetGradient(cv::InputArray img_) {
	cv::Mat img = img_.getMat();
	cv::Size imageSize = img.size();
	
	CV_Assert(img.type() == CV_8UC3);
	
	cv::Mat gradient(imageSize, CV_32F, cv::Scalar(0.0f));
	
	cv::Mat imgGray(imageSize, CV_8U);
	for(int i = 0;i < imageSize.area();i++) {
		imgGray.data[i] = rgb_2_gray(&img.data[i * 3]);
	}

	cv::Mat1b imgGrayPtr = imgGray;
	cv::Mat1f gradientPtr = gradient;
	float grayMinus, grayPlus;
	
	for(int y = 0;y < imageSize.height;y++) {
		grayPlus  = imgGrayPtr(y, 1);
		grayMinus = imgGrayPtr(y, 0);
		gradientPtr(y, 0) = grayPlus - grayMinus + 127.5f;

		for(int x = 1;x < imageSize.width - 1;x++) {
			grayPlus  = imgGrayPtr(y, x + 1);
			grayMinus = imgGrayPtr(y, x - 1);
			gradientPtr(y, x) = 0.5f * (grayPlus - grayMinus) + 127.5f;
		}

		grayPlus  = imgGrayPtr(y, imageSize.width - 1);
		grayMinus = imgGrayPtr(y, imageSize.width - 2);
		gradientPtr(y, imageSize.width - 1) = grayPlus - grayMinus + 127.5f;
	}

	return gradient;
}


cv::Mat CDisparityHelper::GetMatchingCost_CEN(cv::Mat imL, cv::Mat imR, int maxLevel) 
{
	// cvt color to gray
	int m_w = imL.cols;
	int m_h = imL.rows;
	int m_nr_plane = maxLevel;

	Mat lGray, rGray;
	cvtColor( imL, lGray, CV_BGR2GRAY );
	cvtColor( imR, rGray, CV_BGR2GRAY );
	// prepare binary code 
	int H_WD = CENCUS_WND / 2;
	bitset<CENCUS_BIT>* lCode = new bitset<CENCUS_BIT>[ m_w * m_h ];
	bitset<CENCUS_BIT>* rCode = new bitset<CENCUS_BIT>[ m_w * m_h ];
	bitset<CENCUS_BIT>* pLCode = lCode;
	bitset<CENCUS_BIT>* pRCode = rCode;
	for( int y = 0; y < m_h; y ++ ) {
		uchar* pLData = ( uchar* ) ( lGray.ptr<uchar>( y ) );
		uchar* pRData = ( uchar* ) ( rGray.ptr<uchar>( y ) );
		for( int x = 0; x < m_w; x ++ ) {
			int bitCnt = 0;
			for( int wy = - H_WD; wy <= H_WD; wy ++ ) {
				int qy = ( y + wy + m_h ) % m_h;
				uchar* qLData = ( uchar* ) ( lGray.ptr<uchar>( qy ) );
				uchar* qRData = ( uchar* ) ( rGray.ptr<uchar>( qy ) );
				for( int wx = - H_WD; wx <= H_WD; wx ++ ) {
					if( wy != 0 || wx != 0 ) {
						int qx = ( x + wx + m_w ) % m_w;
						( *pLCode )[ bitCnt ] = ( pLData[ x ] > qLData[ qx ] );
						( *pRCode )[ bitCnt ] = ( pRData[ x ] > qRData[ qx ] );
						bitCnt ++;
					}

				}
			}
			pLCode ++;
			pRCode ++;
		}
	}
	// build cost volume
	cv::Size imageSize = imL.size();

	cv::Mat costVol(1, imageSize.area() * maxLevel, CV_32F);
	KIdx_<float, 3> costVolPtr((float *)costVol.data, imageSize.height, imageSize.width, maxLevel);

	bitset<CENCUS_BIT> lB;
	bitset<CENCUS_BIT> rB;
	pLCode = lCode;
	for( int y = 0; y < m_h; y ++ ) {
		int index = y * m_w;
		for( int x = 0; x < m_w; x ++ ) {
			lB = *pLCode;
			for( int d = 0; d < m_nr_plane; d ++ ) {
				costVolPtr(y, x, d) = CENCUS_BIT;
				if( x - d >= 0 ) {
					rB = rCode[ index + x - d ];
					costVolPtr(y, x, d) = ( lB ^ rB ).count();
				}

			}
			pLCode ++;
		}
	}
	delete [] lCode;
	delete [] rCode;

	return costVol;
}


cv::Mat CDisparityHelper::GetMatchingCost(cv::Mat imL, cv::Mat imR, int maxLevel) {
	cv::Mat gradL = GetGradient(imL);
	cv::Mat gradR = GetGradient(imR);

//default: set the same as the non-local cost aggregation from QingXiong Yang in his CVPR 2012 paper
	double max_color_difference = 7;
	double max_gradient_difference = 2;
	double weight_on_color = 0.11;
	double weight_on_gradient = 1.0 - weight_on_color;
//end of default

	cv::Size imageSize = imL.size();
	
	cv::Mat costVol(1, imageSize.area() * maxLevel, CV_32F);
	KIdx_<float, 3> costVolPtr((float *)costVol.data, imageSize.height, imageSize.width, maxLevel);

	cv::Mat imageShifted(imageSize, CV_8UC3);
	cv::Mat gradientShifted(imageSize, CV_32F);

	KIdx_<uchar, 3> imLPtr(imL.data, imageSize.height, imageSize.width, 3);
	KIdx_<uchar, 3> imRPtr(imR.data, imageSize.height, imageSize.width, 3);
	KIdx_<uchar, 3> imShiftPtr(imageShifted.data, imageSize.height, imageSize.width, 3);

	KIdx_<float, 2> gradLPtr((float *)gradL.data, imageSize.height, imageSize.width);
	KIdx_<float, 2> gradRPtr((float *)gradR.data, imageSize.height, imageSize.width);
	KIdx_<float, 2> gShiftPtr((float *)gradientShifted.data, imageSize.height, imageSize.width);

	for(int i = 0;i < maxLevel;i++) {
		//shift the right image by i pixels
		for(int y = 0;y < imageSize.height;y++) {
			memcpy(&imShiftPtr(y, i, 0), &imRPtr(y, 0, 0), sizeof(uchar) * (imageSize.width - i) * 3);
			memcpy(&gShiftPtr(y, i), &gradRPtr(y, 0), sizeof(float) * (imageSize.width - i));
			for(int x = 0;x < i;x++) {
				memcpy(&imShiftPtr(y, x, 0), &imRPtr(y, 0, 0), sizeof(uchar) * 3);
				gShiftPtr(y, x) = gradRPtr(y, 0);
			}
		}

		for(int y = 0;y < imageSize.height;y++) {
			for(int x = 0;x < imageSize.width;x++) {
				double costColor = 0, costGradient;
				
				for(int c = 0; c < 3;c++) costColor += abs(imLPtr(y, x, c) - imShiftPtr(y, x, c));
				costColor = std::min(costColor / 3, max_color_difference);
				
				costGradient = fabs(gradLPtr(y, x) - gShiftPtr(y, x));
				costGradient = std::min(costGradient, max_gradient_difference);
				
				costVolPtr(y, x, i) = float(weight_on_color * costColor + weight_on_gradient * costGradient);
			}
		}
	}
	
	return costVol;
}

cv::Mat CDisparityHelper::GetDisparity_WTA(float *costVol, int w, int h, int maxLevel) {
	cv::Mat disparity(h, w, CV_8U);
	cv::Mat1b disparityPtr = disparity;
	
	KIdx_<float, 3>  costPtr(costVol, h, w, maxLevel);

	for(int y = 0;y < h;y++) {
		for(int x = 0;x < w;x++) {
			int minPos = 0;
			float minValue = costPtr(y, x, minPos);
			
			for(int i = 1;i < maxLevel;i++) {
				if(costPtr(y, x, i) < minValue) {
					minValue = costPtr(y, x, i);
					minPos = i;
				}
			}
			
			disparityPtr(y, x) = (uchar)minPos;
		}
	}

	return disparity;
}

cv::Mat CDisparityHelper::GetRightMatchingCostFromLeft(cv::Mat leftVol, int w, int h, int maxLevel) {
	cv::Mat rightVol = leftVol.clone();
	KIdx_<float, 3>  leftPtr((float*)leftVol.data, h, w, maxLevel);
	KIdx_<float, 3>  rightPtr((float*)rightVol.data, h, w, maxLevel);

	for(int y = 0;y < h;y++) {
		for(int x = 0;x < w - maxLevel;x++) {
			for(int d = 0;d < maxLevel;d++) {
				rightPtr(y, x, d) = leftPtr(y, x+d, d);
			}
		}

		for(int x = w - maxLevel;x < w;x++) {
			for(int d = 0;d < maxLevel;d++) {
				if((x + d) < w) {
					rightPtr(y, x, d) = leftPtr(y, x + d, d);
				} else {
					rightPtr(y, x, d) = rightPtr(y, x, d - 1);
				}
			}
		}
	}

	return rightVol;
}


