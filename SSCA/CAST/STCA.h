#pragma once
#include "..\CommFunc.h"
#include "..\CAMethod.h"

//
// Guided Filter for Cost Aggregatation
//
class STCA :
	public CAMethod
{
public:
	STCA(void)
	{
		printf( "\n\t\tSegment Tree method for cost aggregation" );
	}

	~STCA(void) {}
public:
	void aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
};
