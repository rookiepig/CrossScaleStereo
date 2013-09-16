///////////////////////////////////////////////////////
// File: main.cpp
// Desc: Scale Space Cost Aggregation
// Usage: [CC_METHOD] [CA_METHOD] [USE_MED] [lImg] [rImg] [lDis] [maxDis] [disSc]
// Author: Zhang Kang
// Date: 2013/09/06
///////////////////////////////////////////////////////
#include "CommFunc.h"
#include "SSCA.h"
#include "CC/GrdCC.h"
#include "CAFilter/GFCA.h"
#include "GetMehod.h"


//#define USE_MEDIAN_FILTER

#ifdef USE_MEDIAN_FILTER
#include"CAST/Toolkit.h"
#endif

int main( int argc, char** argv )
{
	printf( "Scale Space Cost Aggregation\n" );
	if( argc != 10 ) {
		printf( "Usage: [CC_METHOD] [CA_METHOD] [PP_METHOD] [C_ALPHA] [lImg] [rImg] [lDis] [maxDis] [disSc]\n" );
		printf( "\nPress any key to continue...\n" );
		getchar();
		return -1;
	}
	string ccName = argv[ 1 ];
	string caName = argv[ 2 ];
	string ppName = argv[ 3 ];
	double costAlpha = atof( argv[ 4 ] );
	string lFn = argv[ 5 ];
	string rFn = argv[ 6 ];
	string lDisFn = argv[ 7 ];
	int maxDis = atoi( argv[ 8 ] );
	int disSc  = atoi( argv[ 9 ] );
	//
	// Load left right image
	//
	printf( "\n--------------------------------------------------------\n" );
	printf( "Load Image: (%s) (%s)\n", argv[ 5 ], argv[ 6 ] );
	printf( "--------------------------------------------------------\n" );
	Mat lImg = imread( lFn, CV_LOAD_IMAGE_COLOR );
	Mat rImg = imread( rFn, CV_LOAD_IMAGE_COLOR );
	if( !lImg.data || !rImg.data ) {
		printf( "Error: can not open image\n" );
		printf( "\nPress any key to continue...\n" );
		getchar();
		return -1;
	}
	// set image format
	cvtColor( lImg, lImg, CV_BGR2RGB );
	cvtColor( rImg, rImg, CV_BGR2RGB );
	lImg.convertTo( lImg, CV_64F, 1 / 255.0f );
	rImg.convertTo( rImg, CV_64F,  1 / 255.0f );

	//
	// Stereo Match at each pyramid
	//
	int PY_LVL = 5;
	// build pyramid and cost volume
	Mat lP = lImg.clone();
	Mat rP = rImg.clone();
	SSCA** smPyr = new SSCA*[ PY_LVL ];
	CCMethod* ccMtd = getCCType( ccName );
	CAMethod* caMtd = getCAType( caName );
	PPMethod* ppMtd = getPPType( ppName );
	for( int p = 0; p < PY_LVL; p ++ ) {
		if( maxDis < 5 ) {
			PY_LVL = p;
			break;
		}
		printf( "\n\tPyramid: %d:", p );
		smPyr[ p ] = new SSCA( lP, rP, maxDis, disSc );


		smPyr[ p ]->CostCompute( ccMtd );

		smPyr[ p ]->CostAggre( caMtd  );
		// pyramid downsample
		maxDis = maxDis / 2 + 1;
		disSc  *= 2;
		pyrDown( lP, lP );
		pyrDown( rP, rP );
	}
	printf( "\n--------------------------------------------------------\n" );
	for( int p = PY_LVL - 2 ; p >= 0; p -- ) {
		smPyr[ p ]->AddPyrCostVol( smPyr[ p + 1 ], costAlpha );
	}
	//
	// Match&Save output
	//
	smPyr[ 0 ]->Match();
	smPyr[ 0 ]->PostProcess( ppMtd );
	Mat lDis = smPyr[ 0 ]->getLDis();
#ifdef USE_MEDIAN_FILTER
	//
	// Median Filter Output
	//
	MeanFilter( lDis, lDis, 3 );
#endif
	imwrite( lDisFn, lDis );

	delete [] smPyr;
	delete ccMtd;
	delete caMtd;
	delete ppMtd;
	return 0;
}
//
//int main( int argc, char** argv )
//{
//	printf( "Normal Cost Aggregation\n" );
//	if( argc != 9 ) {
//		printf( "Usage: [CC_METHOD] [CA_METHOD] [PP_METHOD] [lImg] [rImg] [lDis] [maxDis] [disSc]\n" );
//		printf( "\nPress any key to continue...\n" );
//		getchar();
//		return -1;
//	}
//	string ccName = argv[ 1 ];
//	string caName = argv[ 2 ];
//	string ppName = argv[ 3 ];
//	string lFn = argv[ 4 ];
//	string rFn = argv[ 5 ];
//	string lDisFn = argv[ 6 ];
//	int maxDis = atoi( argv[ 7 ] );
//	int disSc  = atoi( argv[ 8 ] );
//	//
//	// Load left right image
//	//
//	printf( "\n--------------------------------------------------------\n" );
//	printf( "Load Image: (%s) (%s)\n", argv[ 4 ], argv[ 5 ] );
//	printf( "--------------------------------------------------------\n" );
//	Mat lImg = imread( lFn, CV_LOAD_IMAGE_COLOR );
//	Mat rImg = imread( rFn, CV_LOAD_IMAGE_COLOR );
//	if( !lImg.data || !rImg.data ) {
//		printf( "Error: can not open image\n" );
//		printf( "\nPress any key to continue...\n" );
//		getchar();
//		return -1;
//	}
//	// set image format
//	cvtColor( lImg, lImg, CV_BGR2RGB );
//	cvtColor( rImg, rImg, CV_BGR2RGB );
//	lImg.convertTo( lImg, CV_64F, 1 / 255.0f );
//	rImg.convertTo( rImg, CV_64F,  1 / 255.0f );
//
//	//
//	// Stereo Match
//	//
//
//	SSCA* sm = new SSCA( lImg, rImg, maxDis, disSc );
//	CCMethod* ccMtd = getCCType( ccName );
//	CAMethod* caMtd = getCAType( caName );
//	PPMethod* ppMtd = getPPType( ppName );
//	double duration;
//	duration = static_cast<double>(getTickCount());
//
//
//	sm->CostCompute( ccMtd );
//
//	sm->CostAggre( caMtd );
//
//	sm->Match();
//
//	sm->PostProcess( ppMtd );
//
//	duration = static_cast<double>(getTickCount())-duration;
//	duration /= cv::getTickFrequency(); // the elapsed time in sec
//	printf( "\n--------------------------------------------------------\n" );
//	printf( "Total Time: %.2lf s\n", duration );
//	printf( "--------------------------------------------------------\n" );
//	//
//	// Save output
//	//
//	Mat lDis = sm->getLDis();
//#ifdef USE_MEDIAN_FILTER
//	//
//	// Median Filter Output
//	//
//	MeanFilter( lDis, lDis, 3 );
//#endif
//	imwrite( lDisFn, lDis );
//
//	return 0;
//}