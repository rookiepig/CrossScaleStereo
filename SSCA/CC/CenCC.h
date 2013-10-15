#pragma once
#include "..\CommFunc.h"
#include "..\CCMethod.h"

#define CENCUS_WND 9
#define CENCUS_BIT 80

//
// Cencus for Cost Computation
//
class CenCC :
	public CCMethod
{
public:
	CenCC(void) 
	{
		printf( "\n\t\tCencus for Cost Computation" );
	}
	~CenCC(void) {}
public:
	void buildCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
#ifdef COMPUTE_RIGHT
	void buildRightCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* rCostVol );
#endif
};

