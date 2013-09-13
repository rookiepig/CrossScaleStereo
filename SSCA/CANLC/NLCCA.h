#pragma once
#include "..\CommFunc.h"
#include "..\CAMethod.h"

//
// Re compute cost using NLC's implementation
//
#define RE_COMPUTE_COST

//
// Non-local Cost Aggregatation
//
class NLCCA :
	public CAMethod
{
public:
	NLCCA(void)
	{
		printf( "\n\t\tNon-local method for cost aggregation" );
	}

	~NLCCA(void) {}
public:
	void aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
};


