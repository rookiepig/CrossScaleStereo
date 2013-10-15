#pragma once
#include "CommFunc.h"

//
// Interface for Cost Computation
//
class CCMethod
{
public:
	CCMethod(void) {}
	virtual ~CCMethod(void) {}
public:
	///////////////////////////////////////////////////////
	// Func: buildCV
	// Desc: build cost volume
	// In:
	// const Mat& lImg - left color image
	// const Mat& rImg - right color image
	// const int maxDis - maxDis
	// Out:
	// Mat* costVol - out put cost volume
	///////////////////////////////////////////////////////
	virtual void buildCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol ) = 0;
#ifdef COMPUTE_RIGHT
	virtual void buildRightCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* rCostVol ) = 0;
#endif
};

