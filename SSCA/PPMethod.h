#pragma once
#include "CommFunc.h"


//
// Interface for Cost Computation
//
class PPMethod
{
public:
	PPMethod(void) {}
	virtual ~PPMethod(void) {}
public:
	///////////////////////////////////////////////////////
	// Func: buildCV
	// Desc: build cost volume
	// In:
	// const Mat& lImg - left color image
	// const Mat& rImg - right color image
	// const int disSc - disparity scale factor
	// Out:
	// Mat& lDis - output new left disparity
	// Mat& rDis - output new right disparity
	///////////////////////////////////////////////////////
	virtual void postProcess( const Mat& lImg, const Mat& rImg, const int maxDis, const int disSc,
		Mat& lDis, Mat& rDis, Mat& lSeg, Mat& lChk ) = 0;
};

