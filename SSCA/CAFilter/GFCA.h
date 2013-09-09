#pragma once
#include "..\CommFunc.h"
#include "..\CAMethod.h"

//
// Guided Filter for Cost Aggregatation
//
class GFCA :
	public CAMethod
{
public:
	GFCA(void)
	{
		printf( "\n\t\tGF method for cost aggregation" );
	}

	~GFCA(void) {}
public:
	void aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
};
