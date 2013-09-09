#include "GFCA.h"
#include "GuidedFilter.h"


void GFCA::aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol )
{
	// filtering cost volume
	for( int d = 1; d < maxDis; d ++ ) {
		printf( "-c-a" );
		costVol[ d ] = GuidedFilter( lImg, costVol[ d ] );
	}
}
