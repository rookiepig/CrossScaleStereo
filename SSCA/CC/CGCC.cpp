#include "CGCC.h"
#include "../CAFilter/GuidedFilter.h"

inline double cg_myGrdDiff( double* lG, double* rG )
{
	// gradient diff
	double grdDiff = fabs( lG[ 0 ] - rG[ 0 ] );
	grdDiff = grdDiff > GRD_BOUND ? GRD_BOUND : grdDiff;
	return grdDiff;
}
inline double cg_myCostGrd( double* lC, double* rC,
	double* lG, double* rG )
{
	double clrDiff = 0;
	// three color
	for( int c = 0; c < 3; c ++ ) {
		double temp = fabs( lC[ c ] - rC[ c ] );
		clrDiff += temp;
	}
	clrDiff *= 0.3333333333;
	// gradient diff
	double grdDiff = fabs( lG[ 0 ] - rG[ 0 ] );
	clrDiff = clrDiff > CG_TAU_1 ? CG_TAU_1 : clrDiff;
	grdDiff = grdDiff > CG_TAU_2 ? CG_TAU_2 : grdDiff;
	return CG_ALPHA * clrDiff + ( 1 - CG_ALPHA ) * grdDiff;
}
// specail handle for border region
inline double cg_myCostGrd( double* lC, double* lG )
{
	double clrDiff = 0;
	// three color
	for( int c = 0; c < 3; c ++ ) {
		double temp = fabs( lC[ c ] - CG_BORDER_THRES);
		clrDiff += temp;
	}
	clrDiff *= 0.3333333333;
	// gradient diff
	double grdDiff = fabs( lG[ 0 ] - CG_BORDER_THRES );
	clrDiff = clrDiff > CG_TAU_1 ? CG_TAU_1 : clrDiff;
	grdDiff = grdDiff > CG_TAU_2 ? CG_TAU_2 : grdDiff;
	return CG_ALPHA * clrDiff + ( 1 - CG_ALPHA ) * grdDiff;
}

void CGCC::buildCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol )
{
	// for TAD + Grd input image must be CV_64FC3
	CV_Assert( lImg.type() == CV_64FC3 && rImg.type() == CV_64FC3 );

	int hei = lImg.rows;
	int wid = lImg.cols;

	Mat lGray, rGray;
	Mat lGrdX, rGrdX;
	Mat tmp;
	lImg.convertTo( tmp, CV_32F );
	cvtColor( tmp, lGray, CV_RGB2GRAY );
	rImg.convertTo( tmp, CV_32F );
	cvtColor( tmp, rGray, CV_RGB2GRAY );
	// X Gradient
	// sobel size must be 1
	Sobel( lGray, lGrdX, CV_64F, 1, 0, 1 );
	Sobel( rGray, rGrdX, CV_64F, 1, 0, 1 );
	lGrdX += 0.5;
	rGrdX += 0.5;
	// build cost volume! start from 1
	// try 0
	for( int d = 0; d < maxDis; d ++ ) {
		printf( "-c-c-" );
		for( int y = 0; y < hei; y ++ ) {
			double* lData = ( double* ) lImg.ptr<double>( y );
			double* rData = ( double* ) rImg.ptr<double>( y );
			double* lGData = ( double* ) lGrdX.ptr<double>( y );
			double* rGData = ( double* ) rGrdX.ptr<double>( y );
			double* cost   = ( double* ) costVol[ d  ].ptr<double>( y );
			for( int x = 0; x < wid; x ++ ) {
				if( x - d >= 0 ) {
					double* lC = lData + 3 * x;
					double* rC = rData + 3 * ( x - d );
					double* lG = lGData + x;
					double* rG = rGData + x - d;
					cost[ x ] = cg_myCostGrd( lC, rC, lG, rG );
				} else {
					double* lC = lData + 3 * x;
					double* lG = lGData + x;
					cost[ x ] = cg_myCostGrd( lC, lG );
				}

			}
		}
	}

	//
	// add cencus cost volume
	//
	lImg.convertTo( tmp, CV_32F );
	cvtColor( tmp, lGray, CV_RGB2GRAY );
	lGray.convertTo( lGray, CV_8U, 255 );
	rImg.convertTo( tmp, CV_32F );
	cvtColor( tmp, rGray, CV_RGB2GRAY );
	rGray.convertTo( rGray, CV_8U, 255 );
	// prepare binary code 
	int H_WD = CENCUS_WND / 2;
	bitset<CENCUS_BIT>* lCode = new bitset<CENCUS_BIT>[ wid * hei ];
	bitset<CENCUS_BIT>* rCode = new bitset<CENCUS_BIT>[ wid * hei ];
	bitset<CENCUS_BIT>* pLCode = lCode;
	bitset<CENCUS_BIT>* pRCode = rCode;
	for( int y = 0; y < hei; y ++ ) {
		uchar* pLData = ( uchar* ) ( lGray.ptr<uchar>( y ) );
		uchar* pRData = ( uchar* ) ( rGray.ptr<uchar>( y ) );
		for( int x = 0; x < wid; x ++ ) {
			int bitCnt = 0;
			for( int wy = - H_WD; wy <= H_WD; wy ++ ) {
				int qy = ( y + wy + hei ) % hei;
				uchar* qLData = ( uchar* ) ( lGray.ptr<uchar>( qy ) );
				uchar* qRData = ( uchar* ) ( rGray.ptr<uchar>( qy ) );
				for( int wx = - H_WD; wx <= H_WD; wx ++ ) {
					if( wy != 0 || wx != 0 ) {
						int qx = ( x + wx + wid ) % wid;
						( *pLCode )[ bitCnt ] = ( pLData[ x ] > qLData[ qx ] );
						( *pRCode )[ bitCnt ] = ( pRData[ x ] > qRData[ qx ] );
						bitCnt ++;
					}

				}
			}
			pLCode ++;
			pRCode ++;
		}
	}
	// build cost volume
	// Census
	bitset<CENCUS_BIT> lB;
	bitset<CENCUS_BIT> rB;
	pLCode = lCode;
	for( int y = 0; y < hei; y ++ ) {
		int index = y * wid;
		for( int x = 0; x < wid; x ++ ) {
			lB = *pLCode;
			for( int d = 0; d < maxDis; d ++ ) {
				double* cost   = ( double* ) costVol[ d ].ptr<double>( y );
				double tmpCost = CENCUS_BIT;
				if( x - d >= 0 ) {
					rB = rCode[ index + x - d ];
					tmpCost = ( lB ^ rB ).count();
				}
				// normalise census cost
				// tmpCost /= CENCUS_BIT;
				// cost[ x ] = cost[ x ] + LAMBDA_CEN * tmpCost;
				// AD - Cencus
				cost[ x ] = 2 - exp( - tmpCost / 35 ) - exp( - 255 * cost[ x ] / 15 );

			}
			pLCode ++;
		}
	}
	delete [] lCode;
	delete [] rCode;

}
#ifdef COMPUTE_RIGHT
void CGCC::buildRightCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* rCostVol )
{
	printf( "!!!!Error!!!! CGCC do not implement right CV\n" );
}
#endif