#include "BFCA.h"
#include "BilateralFilter.h"


void BFCA::aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol )
{
	// filtering cost volume
	for( int d = 1; d < maxDis; d ++ ) {
		printf( "-c-a" );
		// change BF window size for KITTI
		costVol[ d ] = BilateralFilter( lImg, costVol[ d ], 35 );
	}
}
