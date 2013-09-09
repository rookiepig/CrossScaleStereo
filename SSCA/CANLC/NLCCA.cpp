#include "NLCCA.h"
#include "qx_nonlocal_cost_aggregation.h"


// convert Mat to QX Image
void cvtMatQX( const Mat& img, unsigned char* qxImg )
{
	Mat t;
	img.convertTo( t, CV_8U, 255 );
	// t is RGB!!! CV_8UC3
	for( int y = 0; y < img.rows; y ++ ) {
		uchar* tData = ( uchar* ) t.ptr<uchar>( y );
		for( int x = 0; x < img.cols; x ++ ) {
			qxImg[ 0 ] = tData[ 0 ];
			qxImg[ 1 ] = tData[ 1 ];
			qxImg[ 2 ] = tData[ 2 ];
			qxImg += 3;
			tData += 3;
		}
	}
}

//
// Non-local Cost Aggregatation
//
void NLCCA::aggreCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol )
{
	printf( "\n\t\tNon-local cost aggregation" );
	printf( "\n\t\tCost volume need to be recompute" );
	// construct non-local aggregation class
	double sigma= 0.1;
	int h = lImg.rows;
	int w = lImg.cols;
	// init nlca
	unsigned char*** left=qx_allocu_3(h,w,3);//allocate memory
	unsigned char*** right=qx_allocu_3(h,w,3);
	cvtMatQX( lImg, left[ 0 ][ 0 ] );
	cvtMatQX( rImg, right[ 0 ][ 0 ] );
	qx_nonlocal_cost_aggregation m_nlca;
	m_nlca.init(h,w,maxDis,sigma);//initialization
	// copy image data
	m_nlca.m_left = left;
	m_nlca.m_right = right;
	// recompute cost volume
	// m_nlca.matching_cost_from_color_and_gradient( left, right );
	// build tree
	m_nlca.m_tf.build_tree(left[0][0]);
	//// my cost volume -> nlca
	//// be careful yours [1,maxDis-1]
	//// nlca [0,maxDis-1]
	// this just be used for Cencus Cost
	for( int d = 0; d < maxDis; d ++ ) {
		printf( "-n-m-" );
		for( int y = 0; y < h; y ++ ) {
			double* cost   = ( double* ) costVol[ d ].ptr<double>( y );
			for( int x = 0; x < w; x ++ ) {
				m_nlca.m_cost_vol[ y ][ x ][ d ] = cost[ x ];
			}
		}
	}
	// filter cost volume
	m_nlca.m_tf.filter( m_nlca.m_cost_vol[0][0],
		m_nlca.m_cost_vol_temp[0][0],
		m_nlca.m_nr_plane
		);

	// nlca -> my
	// be careful yours [1,maxDis-1]
	// nlca [0,maxDis-1]
	for( int d = 1; d < maxDis; d ++ ) {
		printf( "*m*n*" );
		for( int y = 0; y < h; y ++ ) {
			double* cost   = ( double* ) costVol[ d ].ptr<double>( y );
			for( int x = 0; x < w; x ++ ) {
				cost[ x ] = m_nlca.m_cost_vol[ y ][ x ][ d ];

			}
		}
	}

	qx_freeu_3(left); 
	left=NULL;//free memory
	qx_freeu_3(right); 
	right=NULL;
}