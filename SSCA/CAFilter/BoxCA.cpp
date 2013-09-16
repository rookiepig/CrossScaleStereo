#include "BoxCA.h"
#include "GuidedFilter.h"


void BoxCA::aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol )
{
	// filtering cost volume
	for( int d = 1; d < maxDis; d ++ ) {
		printf( "-c-a" );
		// 7 x 7 box filter
		costVol[ d ] = BoxFilter( costVol[ d ], 3 );
	}
}
