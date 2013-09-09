#pragma once
#include "..\CommFunc.h"
#include "..\CAMethod.h"

//
// Box Filter for Cost Aggregatation
//
class BoxCA :
	public CAMethod
{
public:
	BoxCA(void)
	{
		printf( "\n\t\Box Filter for cost aggregation" );
	}

	~BoxCA(void) {}
public:
	void aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
};
