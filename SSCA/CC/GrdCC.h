#pragma once
#include "..\CommFunc.h"
#include "..\CCMethod.h"

// CVPR 11
#define BORDER_THRES 0.011764

#define TAU_1 0.02745
#define TAU_2 0.00784
#define ALPHA 0.11

//
// TAD + GRD for Cost Computation
//
class GrdCC :
	public CCMethod
{
public:
	GrdCC(void) 
	{
		printf( "\n\t\tGRD method for Cost Computation" );
	}
	~GrdCC(void) {}
public:
	void buildCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol );
#ifdef COMPUTE_RIGHT
	void buildRightCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* rCostVol );
#endif
};

