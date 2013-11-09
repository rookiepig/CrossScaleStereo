#include "WMPP.h"

namespace WMPP_FUNC{
	void lrCheck( Mat& lDis, Mat& rDis, int* lValid, int* rValid, const int disSc )
	{
		int hei = lDis.rows;
		int wid = lDis.cols;
		int imgSize = hei * wid;
		memset( lValid, 0, imgSize * sizeof( int ) );
		memset( rValid, 0, imgSize * sizeof( int ) );
		int* pLValid = lValid;
		int* pRValid = rValid;
		for( int y = 0; y < hei; y ++ ) {
			uchar* lDisData = ( uchar* ) lDis.ptr<uchar>( y );
			uchar* rDisData = ( uchar* ) rDis.ptr<uchar>( y );
			for( int x = 0; x < wid; x ++ ) {
				// check left image
				int lDep = lDisData[ x ] / disSc;
				// assert( ( x - lDep ) >= 0 && ( x - lDep ) < wid );
				int rLoc = ( x - lDep + wid ) % wid;
				int rDep = rDisData[ rLoc ] / disSc;
				// disparity should not be zero
				if( lDep == rDep && lDep >= 2 ) {
					*pLValid = 1;
				}
				// check right image
				rDep = rDisData[ x ] / disSc;
				// assert( ( x + rDep ) >= 0 && ( x + rDep ) < wid );
				int lLoc = ( x + rDep + wid ) % wid;
				lDep = lDisData[ lLoc ] / disSc;
				// disparity should not be zero
				if( rDep == lDep && rDep >= 2 ) {
					*pRValid = 1;
				}
				pLValid ++;
				pRValid ++;
			}
		}
	}
	void fillInv( Mat& lDis, Mat& rDis, int* lValid, int* rValid )
	{
		int hei = lDis.rows;
		int wid = lDis.cols;
		// fill left dep
		int* pLValid = lValid;
		for( int y = 0; y < hei; y ++ ) {
			int* yLValid = lValid + y * wid;
			uchar* lDisData = ( uchar* ) lDis.ptr<uchar>( y );
			for( int x = 0; x < wid; x ++ ) {
				if( *pLValid == 0 ) {
					// find left first valid pixel
					int lFirst = x;
					int lFind = 0;
					while( lFirst >= 0 ) {
						if( yLValid[ lFirst ] ) {
							lFind = 1;
							break;
						}
						lFirst --;
					}
					int rFind = 0;
					// find right first valid pixel
					int rFirst = x;
					while( rFirst < wid ) {
						if( yLValid[ rFirst ] ) {
							rFind = 1;
							break;
						}
						rFirst ++;
					}
					// set x's depth to the lowest one
					if( lFind && rFind ) {
						if( lDisData[ lFirst ] <= lDisData[ rFirst ] ) {
							lDisData[ x ] = lDisData[ lFirst ];
						} else {
							lDisData[ x ] = lDisData[ rFirst ];
						}
					} else if( lFind ) {
						lDisData[ x ] = lDisData[ lFirst ];
					} else if ( rFind ) {
						lDisData[ x ] = lDisData[ rFirst ];
					}

				}
				pLValid ++;
			}
		}
		// fill right dep
		int* pRValid = rValid;
		for( int y = 0; y < hei; y ++ ) {
			int* yRValid = rValid + y * wid;
			uchar* rDisData = ( uchar* ) ( rDis.ptr<uchar>( y ) );
			for( int x = 0; x < wid; x ++ ) {
				if( *pRValid == 0 ) {
					// find left first valid pixel
					int lFirst = x;
					int lFind = 0;
					while( lFirst >= 0 ) {
						if( yRValid[ lFirst ] ) {
							lFind = 1;
							break;
						}
						lFirst --;
					}
					// find right first valid pixel
					int rFirst = x;
					int rFind = 0;
					while( rFirst < wid ) {
						if( yRValid[ rFirst ] ) {
							rFind = 1;
							break;
						}
						rFirst ++;
					}
					if( lFind && rFind ) {
						// set x's depth to the lowest one
						if( rDisData[ lFirst ] <= rDisData[ rFirst ] ) {
							rDisData[ x ] = rDisData[ lFirst ];
						} else {
							rDisData[ x ] = rDisData[ rFirst ];
						}
					} else if( lFind ) {
						rDisData[ x ] = rDisData[ lFirst ];
					} else if ( rFind )  {
						rDisData[ x ] = rDisData[ rFirst ];
					}

				}
				pRValid ++;
			}
		}
	}

	void wgtMedian( const Mat& lImg, const Mat& rImg, Mat& lDis, Mat& rDis, int* lValid, int* rValid, const int maxDis, const int disSc )
	{
		int hei = lDis.rows;
		int wid = lDis.cols;
		int wndR = MED_SZ / 2;
		double* disHist = new double[ maxDis ];

		// filter left
		int* pLValid = lValid;
		for( int y = 0; y < hei; y ++  ) {
			uchar* lDisData = ( uchar* ) lDis.ptr<uchar>( y );
			float* pL = ( float* ) lImg.ptr<float>( y );
			for( int x = 0; x < wid; x ++ ) {
				if( *pLValid == 0 ) {
					// just filter invalid pixels
					memset( disHist, 0, sizeof( double ) * maxDis );
					double sumWgt = 0.0f;
					// set disparity histogram by bilateral weight
					for( int wy = - wndR; wy <= wndR; wy ++ ) {
						int qy = ( y + wy + hei ) % hei;
						// int* qLValid = lValid + qy * wid;
						float* qL = ( float* ) lImg.ptr<float>( qy );
						uchar* qDisData = ( uchar* ) lDis.ptr<uchar>( qy );
						for( int wx = - wndR; wx <= wndR; wx ++ ) {
							int qx = ( x + wx + wid ) % wid;
							// invalid pixel also used
							// if( qLValid[ qx ] && wx != 0 && wy != 0 ) {
							int qDep = qDisData[ qx ] / disSc;
							if( qDep != 0 ) {

								double disWgt = wx * wx + wy * wy;
								// disWgt = sqrt( disWgt );
								double clrWgt = ( pL[ 3 * x ] - qL[ 3 * qx ] ) * ( pL[ 3 * x ] - qL[ 3 * qx ] ) +
									( pL[ 3 * x + 1 ] - qL[ 3 * qx + 1 ] ) * ( pL[ 3 * x + 1 ] - qL[ 3 * qx + 1 ] ) +
									( pL[ 3 * x + 2 ] - qL[ 3 * qx + 2 ] ) * ( pL[ 3 * x + 2 ] - qL[ 3 * qx + 2 ] );
								// clrWgt = sqrt( clrWgt );
								double biWgt = exp( - disWgt / ( SIG_DIS * SIG_DIS ) - clrWgt / ( SIG_CLR * SIG_CLR ) );
								disHist[ qDep ] += biWgt;
								sumWgt += biWgt;
							}
							// }
						}
					}
					double halfWgt = sumWgt / 2.0f;
					sumWgt = 0.0f;
					int filterDep = 0;
					for( int d = 0; d < maxDis; d ++ ) {
						sumWgt += disHist[ d ];
						if( sumWgt >= halfWgt ) {
							filterDep = d;
							break;
						}
					}
					// set new disparity
					lDisData[ x ] = filterDep * disSc;
				}
				pLValid ++;
			}
		}
		// filter right depth
		int* pRValid = rValid;
		for( int y = 0; y < hei; y ++  ) {
			uchar* rDisData = ( uchar* ) rDis.ptr<uchar>( y );
			float* pR = ( float* ) rImg.ptr<float>( y );
			for( int x = 0; x < wid; x ++ ) {
				if( *pRValid == 0 ) {
					// just filter invalid pixels
					memset( disHist, 0, sizeof( double ) * maxDis );
					double sumWgt = 0.0f;
					// set disparity histogram by bilateral weight
					for( int wy = - wndR; wy <= wndR; wy ++ ) {
						int qy = ( y + wy + hei ) % hei;
						// int* qRValid = rValid + qy * wid;
						float* qR = ( float* ) rImg.ptr<float>( qy );
						uchar* qDisData = ( uchar* ) rDis.ptr<uchar>( qy );
						for( int wx = - wndR; wx <= wndR; wx ++ ) {
							int qx = ( x + wx + wid ) % wid;
							// if( qRValid[ qx ] && wx != 0 && wy != 0 ) {
							int qDep = qDisData[ qx ] / disSc;
							if( qDep != 0 ) {

								double disWgt = wx * wx + wy * wy;
								disWgt = sqrt( disWgt );
								double clrWgt =
									( pR[ 3 * x ] - qR[ 3 * qx ] ) * ( pR[ 3 * x ] - qR[ 3 * qx ] ) +
									( pR[ 3 * x + 1 ] - qR[ 3 * qx + 1 ] ) * ( pR[ 3 * x + 1 ] - qR[ 3 * qx + 1 ] ) +
									( pR[ 3 * x + 2 ] - qR[ 3 * qx + 2 ] ) * ( pR[ 3 * x + 2 ] - qR[ 3 * qx + 2 ] );
								clrWgt = sqrt( clrWgt );
								double biWgt = exp( - disWgt / ( SIG_DIS * SIG_DIS ) - clrWgt / ( SIG_CLR * SIG_CLR ) );
								disHist[ qDep ] += biWgt;
								sumWgt += biWgt;
							}
							// }
						}
					}
					double halfWgt = sumWgt / 2.0f;
					sumWgt = 0.0f;
					int filterDep = 0;
					for( int d = 0; d < maxDis; d ++ ) {
						sumWgt += disHist[ d ];
						if( sumWgt >= halfWgt ) {
							filterDep = d;
							break;
						}
					}
					// set new disparity
					rDisData[ x ] = filterDep * disSc;
				}
				pRValid ++;
			}
		}

		delete [] disHist;
	}
	void saveChk( const int hei, const int wid,  int* lValid, int* rValid )
	{
		Mat lChk = Mat::zeros( hei, wid, CV_8UC1 );
		Mat rChk = Mat::zeros( hei, wid, CV_8UC1 );
		int* pLV = lValid;
		int* pRV = rValid;
		for( int y = 0; y < hei; y ++ ) {
			uchar* lChkData = ( uchar* )( lChk.ptr<uchar>( y ) );
			uchar* rChkData = ( uchar* )( rChk.ptr<uchar>( y ) );
			for( int x = 0; x < wid; x ++ ) {
				if( *pLV ) {
					lChkData[ x ] = 0;
				} else{
					lChkData[ x ] = 255;
				}

				if( *pRV ) {
					rChkData[ x ] = 0;
				} else{
					rChkData[ x ] = 255;
				}
				pLV ++;
				pRV ++;
			}
		}
		imwrite( "l_chk.png", lChk );
		imwrite( "r_chk.png", rChk );
	}
}

void WMPP::postProcess( const Mat& lImg, const Mat& rImg, const int maxDis, const int disSc, Mat& lDis, Mat& rDis,
	Mat& lSeg, Mat& lChk )
{
	// color image should be 3x3 median filtered
	// according to weightedMedianMatlab.m from CVPR11
	Mat lTmp, rTmp;
	lImg.convertTo( lTmp, CV_32F );
	rImg.convertTo( rTmp, CV_32F );
	int hei = lDis.rows;
	int wid = lDis.cols;
	int imgSize = hei * wid;
	int* lValid = new int[ imgSize ];
	int* rValid = new int[ imgSize ];
	
	// iter 3 times
    for( int i = 0; i < 3; i ++ ) {
		// save check results
		WMPP_FUNC::lrCheck( lDis, rDis, lValid, rValid, disSc );
		WMPP_FUNC::fillInv( lDis, rDis, lValid, rValid );
		WMPP_FUNC::wgtMedian( lTmp, rTmp, lDis, rDis, lValid, rValid, maxDis, disSc );
	}
	// see last check results
	WMPP_FUNC::lrCheck( lDis, rDis, lValid, rValid, disSc );
	WMPP_FUNC::saveChk( hei, wid, lValid, rValid );
	delete [] lValid;
	delete [] rValid;
}
