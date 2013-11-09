#pragma once
#include "..\CommFunc.h"
#include "..\CCMethod.h"

// CVPR 11
#define BORDER_THRES 0.011764

#define TAU_1 0.02745
#define TAU_2 0.00784
#define ALPHA 0.11

// dong bo
//#define BORDER_THRES 0.0127966
//#define TAU_1 0.0529
//#define TAU_2 0.00784
//#define ALPHA 0.11


// Best_ST
//#define BORDER_THRES 0.02666666666666666666666666666667
//
//#define TAU_1 0.05490196078431372549019607843137
//#define TAU_2 0.01960784313725490196078431372549
//#define ALPHA 0.2

// Best_NL
//#define BORDER_THRES 0.02223529411764705882352941176471
//
//#define TAU_1 0.04313725490196078431372549019608
//#define TAU_2 0.01764705882352941176470588235294
//#define ALPHA 0.18

// Best_GF
//#define BORDER_THRES 0.01313725490196078431372549019608
//#define TAU_1 0.04313725490196078431372549019608
//#define TAU_2 0.00980392156862745098039215686275
//#define ALPHA 0.1

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

