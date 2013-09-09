#pragma once
#include "CommFunc.h"


//
// Interface for Cost Aggregtation
//
class CAMethod 
{
public:
	CAMethod(void) {}
	virtual ~CAMethod(void) {}
public:
	///////////////////////////////////////////////////////
	// Func: aggreCV
	// Desc: aggregate cost volume
	// In:
	// const Mat& lImg - left color image
	// const Mat& rImg - right color image
	// const int maxDis - maxDis
	// Out:
	// Mat* costVol - out put cost volume
	///////////////////////////////////////////////////////
	virtual void aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol ) = 0;
};
