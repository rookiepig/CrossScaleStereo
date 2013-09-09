#include "BilateralFilter.h"

Mat BilateralFilter( const Mat& I, const Mat& p, const int wndSZ /*= 9*/, double sig_sp /*= 5*/, const double sig_clr /*= 0.028 */ )
{
	// filter signal must be 1 channel
	CV_Assert( p.type() == CV_64FC1 );

	// range parameter is half window size
	sig_sp = wndSZ / 2.0f;

	int H = I.rows;
	int W = I.cols;
	int H_WD = wndSZ / 2;
	Mat dest = p.clone();
	if( 1 == I.channels() ) {
		for( int y = 0; y < H; y ++ ) {
			double* pData =  ( double* ) ( p.ptr<double>( y ) );
			double* destData =  ( double* ) ( dest.ptr<double>( y ) );
			double* IP    =  ( double* ) ( I.ptr<double>( y ) );
			for( int x = 0; x < W; x ++ ) {
				double sum = 0.0f;
				double sumWgt = 0.0f;
				for( int  wy = - H_WD; wy <= H_WD; wy ++ ) {
					int qy =  y + wy;
					if( qy < 0 ) {
						qy += H;
					}
					if( qy >= H ) {
						qy -= H;
					}
					double* qData = ( double* ) ( p.ptr<double>( qy ) );
					double* IQ    = ( double* ) ( I.ptr<double>( qy ) );
					for( int wx = - H_WD; wx <= H_WD; wx ++ ) {
						int qx = x + wx;
						if( qx < 0 ) {
							qx += W;
						}
						if( qx >= W ) {
							qx -= W;
						}
						double spDis = wx * wx + wy * wy;
						double clrDis = fabs( IQ[ qx ] - IP[ x ] );
						double wgt = exp( - spDis / ( sig_sp * sig_sp ) - clrDis * clrDis / ( sig_clr * sig_clr ) );
						sum += wgt * qData[ qx ];
						sumWgt += wgt;
					}
				}
				destData[ x ] = sum / sumWgt;
			}
		}
	} else if( 3 == I.channels() ) {
		for( int y = 0; y < H; y ++ ) {
			double* pData =  ( double* ) ( p.ptr<double>( y ) );
			double* destData =  ( double* ) ( dest.ptr<double>( y ) );
			double* IP    =  ( double* ) ( I.ptr<double>( y ) );
			for( int x = 0; x < W; x ++ ) {
				double* pClr = IP + 3 * x;
				double sum = 0.0f;
				double sumWgt = 0.0f;
				for( int  wy = - H_WD; wy <= H_WD; wy ++ ) {
					int qy =  y + wy;
					if( qy < 0 ) {
						qy += H;
					}
					if( qy >= H ) {
						qy -= H;
					}
					double* qData = ( double* ) ( p.ptr<double>( qy ) );
					double* IQ    = ( double* ) ( I.ptr<double>( qy ) );
					for( int wx = - H_WD; wx <= H_WD; wx ++ ) {
						int qx = x + wx;
						if( qx < 0 ) {
							qx += W;
						}
						if( qx >= W ) {
							qx -= W;
						}
						double* qClr = IQ + 3 * qx;
						double spDis = wx * wx + wy * wy;
						double clrDis = 0.0f;
						for( int c = 0; c < 3; c++ ) {
							clrDis += fabs( pClr[ c ] - qClr[ c ] );
						}
						clrDis *= 0.333333333;
						double wgt = exp( - spDis / ( sig_sp * sig_sp ) - clrDis * clrDis / ( sig_clr * sig_clr ) );
						sum += wgt * qData[ qx ];
						sumWgt += wgt;
					}
				}
				destData[ x ] = sum / sumWgt;
			}
		}
	}
	return dest;
}

