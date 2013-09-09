#pragma once
#include "..\CommFunc.h"
#include "..\CAMethod.h"

//
// Bilateral Filter for Cost Aggregatation
//
class BFCA :
	public CAMethod
{
public:
	BFCA(void)
	{
		printf( "\n\t\Bilateral Filter for cost aggregation" );
	}

	~BFCA(void) {}
public:
	void aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
};
