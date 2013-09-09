#include "STCA.h"
#include "StereoDisparity.h"
#include "StereoHelper.h"
#include "SegmentTree.h"

//
// Segment-Tree Cost Aggregation
//
void STCA::aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol )
{
	printf( "\n\t\tSegment Tree cost aggregation" );
	printf( "\n\t\tCost volume need to be recompute" );

	int hei = lImg.rows;
	int wid = lImg.cols;
	// image format must convert
	Mat lSgImg, rSgImg;
	lImg.convertTo( lSgImg, CV_8U, 255 );
	rImg.convertTo( rSgImg, CV_8U, 255 );
	cvtColor( lSgImg, lSgImg, CV_RGB2BGR );
	cvtColor( rSgImg, rSgImg, CV_RGB2BGR );
	Mat sgLCost;
	CDisparityHelper dispHelper;
	// init segmentation tree cost volume
	sgLCost = Mat::zeros(1, hei * wid * maxDis, CV_32F);
	CV_Assert(lSgImg.type() == CV_8UC3 && rSgImg.type() == CV_8UC3);
	// recompute cost volume
	//sgLCost = 
	//	dispHelper.GetMatchingCost( lSgImg, rSgImg, maxDis );
	// my cost to st
	// !!! mine start from 1
	// just used for cencus cost
	float* pCV = ( float* )sgLCost.data;
	for( int y = 0; y < hei; y ++ ) {
		for( int x = 0; x < wid; x ++ ) {
			for( int d = 0; d < maxDis; d ++ ) {
				double* cost   = ( double* ) costVol[ d ].ptr<double>( y );
				*pCV = cost[ x ];
				pCV ++;
			}
		}
	}

	// build tree
	CSegmentTree stree;
	CColorWeight cWeight( lSgImg );
	stree.BuildSegmentTree( lSgImg, 0.1, 1200, cWeight);
	// filter cost volume
	stree.Filter(sgLCost, maxDis);

	// st cost to my
	pCV = ( float* )sgLCost.data;
	for( int y = 0; y < hei; y ++ ) {
		for( int x = 0; x < wid; x ++ ) {
			for( int d = 0; d < maxDis; d ++ ) {
				double* cost   = ( double* ) costVol[ d ].ptr<double>( y );
				cost[ x ]  = *pCV;
				pCV ++;
			}
		}
	}
}

