#pragma once
#include "..\CommFunc.h"
#include "..\CCMethod.h"

#define CENCUS_WND 9
#define CENCUS_BIT 80
#define LAMBDA_CEN 0.5
#define GRD_BOUND  0.1

//
// Cencus + Gradient Cost
//
class CGCC :
	public CCMethod
{
public:
	CGCC(void) 
	{
		printf( "\n\t\tCencus + Gradient for Cost Computation" );
	}
	~CGCC(void) {}
public:
	void buildCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
};

