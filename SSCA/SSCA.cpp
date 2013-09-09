#include "CommFunc.h"
#include "SSCA.h"

SSCA::SSCA(const Mat l, const Mat r, const int m, const int d)
	: lImg( l ), rImg( r ), maxDis( m ), disSc( d )
{
	// all input image must be in CV_64FC3 format
	CV_Assert( lImg.type() == CV_64FC3 && rImg.type() == CV_64FC3 );
	wid = lImg.cols;
	hei = lImg.rows;
	// init disparity map
	lDis = Mat::zeros( hei, wid, CV_8UC1 );

	// init cost volum data
	costVol = new Mat[ maxDis  ];
	for( int mIdx = 0; mIdx < maxDis; mIdx ++ ) {
		costVol[ mIdx ] = Mat::zeros( hei, wid, CV_64FC1 );
	}
}



SSCA::~SSCA(void)
{
	delete [] costVol;
}
// get left disparity
Mat SSCA::getLDis()
{
	return lDis;
}
//
// Save Cost Volume
//
void SSCA::saveCostVol(const string fn)
{
	// save as matlab order
	FILE* fp = fopen( fn.c_str(), "w" );
	for( int d = 1; d < maxDis; d ++ ) {
		printf( "-s-v-" );
		for( int x = 0; x < wid; x ++ ) {
			for( int y = 0; y < hei; y ++ ) {
				double* cost = costVol[ d ].ptr<double>( y );
				fprintf( fp, "%lf\n", cost[ x ] );
			}
		}
	}
	fclose( fp );
}
//
// Add previous cost volume from pyramid
//
void SSCA::AddPyrCostVol(SSCA *pre)
{
	printf( "\n\tAdd Pyramid Cost:" );
	for( int d = 1; d < maxDis; d ++ ) {
		int pD = ( d + 1 ) / 2;
		printf( ".a.p." );
		for( int y = 0; y < hei; y ++ ) {
			int pY = y / 2;
			double* cost = costVol[ d ].ptr<double>( y );
			double* pCost = pre->costVol[ pD ].ptr<double>( pY );
			for( int x = 0; x < wid; x ++ ) {
				int pX = x / 2;
				cost[ x ] = COST_ALPHA * cost[ x ] +
					( 1 - COST_ALPHA ) * pCost[ pX ];

			}
		}
	}
}
//
// 1. Cost Computation
//
void SSCA::CostCompute( CCMethod* ccMtd )
{
	printf( "\n\tCost Computation:" );
	if( ccMtd ) {
		ccMtd->buildCV( lImg, rImg, maxDis, costVol );
	} else {
		printf( "\n\t\tDo nothing" );
	}

}
//
// 2. Cost Aggregation
//
void SSCA::CostAggre( CAMethod* caMtd )
{
	printf( "\n\tCost Aggregation:" );
	if( caMtd ) {
		caMtd->aggreCV( lImg, rImg, maxDis, costVol );
	} else {
		printf( "\n\t\tDo nothing" );
	}
	
}
//
// 3. Match
//
void SSCA::Match( void )
{
	printf( "\n\tMatch" );
	for( int y = 0; y < hei; y ++ ) {
		uchar* lDisData = ( uchar* ) lDis.ptr<uchar>( y );
		for( int x = 0; x < wid; x ++ ) {
			double minCost = DOUBLE_MAX;
			int    minDis  = 0;
			for( int d = 1; d < maxDis; d ++ ) {
				double* costData = ( double* )costVol[ d ].ptr<double>( y );
				if( costData[ x ] < minCost ) {
					minCost = costData[ x ];
					minDis  = d;
				}
			}
			lDisData[ x ] = minDis * disSc;
		}
	}
}
//
// 4. Post Process;
//
void SSCA::PostProcess( PPMethod* ppMtd )
{
	printf( "\n\tPostProcess:" );
	if( ppMtd ) {

	} else {
		printf( "\n\t\tDo nothing" );
	}
}
