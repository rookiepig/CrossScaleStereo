#pragma once
#include "..\CommFunc.h"
#include "..\PPMethod.h"

#define MED_SZ 35
#define SIG_CLR 0.1
#define SIG_DIS 17

#define SEG_SIGMA 1.0
#define SEG_K 50
#define SEG_MIN 80
#define SEG_HIST 10000

#define MIN_FIT_SZIE 1000
#define DIFF_FIT_CUT 200
#define SMALL_PERC   0.1
#define BIG_PERC     0.4

//
// Segment-based Post-processing
//
class SGPP :
	public PPMethod
{
public:
	SGPP(void) 
	{
		printf( "\n\t\tSegment-based Post-processing" );
	}
	~SGPP(void) {}
public:
	void postProcess( const Mat& lImg, const Mat& rImg, const int maxDis, const int disSc,
		Mat& lDis, Mat& rDis, Mat& lSeg, Mat& lChk );
};
