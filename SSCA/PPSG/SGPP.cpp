#include "SGPP.h"
#include <cstdio>
#include <cstdlib>
#include "image.h"
#include "misc.h"
#include "pnmfile.h"
#include "segment-image.h"

namespace SGPP_FUNC {
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
				if( lDep == rDep && lDep > 3 && lDep < 150 ) {
					*pLValid = 1;
				}
				// check right image
				rDep = rDisData[ x ] / disSc;
				// assert( ( x + rDep ) >= 0 && ( x + rDep ) < wid );
				int lLoc = ( x + rDep + wid ) % wid;
				lDep = lDisData[ lLoc ] / disSc;
				// disparity should not be zero
				if( lDep == rDep  && rDep > 3 && rDep < 150 ) {
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
	void saveChk( const int hei, const int wid,  int* lValid, int* rValid, Mat& lChk )
	{
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
#ifdef _DEBUG
		imwrite( "l_chk.png", lChk );
		imwrite( "r_chk.png", rChk );
#endif
	}
	//
	// Convert Mat to Image
	//
	void matToImage( const Mat& mat, image<rgb>*& input )
	{
		int hei = mat.rows;
		int wid = mat.cols;
		rgb c;
		for( int y = 0;y < hei; y ++ ) {
			uchar* matData = ( uchar* )mat.ptr<uchar>( y );
			for( int x = 0; x < wid; x ++ ) {
				// mat must be RGB !!!
				c.r = matData[ 3 * x ];
				c.g = matData[ 3 * x + 1 ];
				c.b = matData[ 3 * x + 2 ];
				imRef( input, x, y ) = c;
			}
		}
	}
	void imageToMat( image<rgb>*& input, Mat& mat )
	{
		int hei = mat.rows;
		int wid = mat.cols;
		rgb c;
		for( int y = 0;y < hei; y ++ ) {
			uchar* matData = ( uchar* )mat.ptr<uchar>( y );
			for( int x = 0; x < wid; x ++ ) {
				// RGB -> BGR
				c = imRef( input, x, y );
				matData[ 3 * x ] = c.b;
				matData[ 3 * x + 1 ] = c.g;
				matData[ 3 * x + 2 ] = c.r;
			}
		}

	}
	void histFitOneSeg( MySegment& curS, const Mat& lDis, const Mat& lDGrdX, const Mat& lDGrdY, int* lValid )
	{
		int hei = lDis.rows;
		int wid = lDis.cols;
		vector<double> param[ 3 ];
		double minParam[ 3 ] = { 1e10, 1e10, 1e10 };
		double maxParam[ 3 ] = { -1e10, -1e10, -1e10 };
		for( int i = 0; i < curS.xIdx.size(); i ++ ) {
			// only valid pixel can be used
			int curY = curS.yIdx[ i ];
			int curX = curS.xIdx[ i ];
			if( lValid[ curY * wid + curX ] ) {
				double curParam[ 3 ] = {0};
				curParam[ 0 ] = lDGrdX.at<double>( curY,
					curX );
				curParam[ 1 ]= lDGrdY.at<double>(curY,
					curX );
				double curD = ( double ) lDis.at<uchar>( curY,
					curX );
				curParam[ 2 ] = curD - curParam[ 0 ] * curX 
					- curParam[ 1 ] * curY;
				for( int p = 0; p < 3; p ++ ) {
					if( curParam[ p ] < minParam[ p ] ) {
						minParam[ p ] = curParam[ p ];
					}
					if( curParam[ p ] > maxParam[ p ] ) {
						maxParam[ p ] = curParam[ p ];
					}
					param[ p ].push_back( curParam[ p ] );
				}
			}
		}
		// build parameter histogram
		int hist[ 3 ][ SEG_HIST + 1 ];
		memset( &( hist[ 0 ][ 0 ] ), 0, 3 * SEG_HIST * sizeof( int ) );
		double itv[ 3 ] = {0};
		for( int p = 0; p < 3; p ++ ) {
			itv[ p ] = ( maxParam[ p ] - minParam[ p ] ) / SEG_HIST;
		}
		for( int p = 0; p < 3; p ++ ) {
			for( int i = 0; i < param[ p ].size(); i ++ ) {
				int hIdx = 0;
				if( itv[ p ] != 0 ) {
					hIdx = ( int )( ( param[ p ][ i ] - minParam[ p ] ) /
						itv[ p ] );
				}
				if( hIdx < 0 || hIdx >= SEG_HIST + 1 ) {
					printf( "error " );
				}
				hist[ p ][ hIdx ] ++;
			}
		}
		// find max in histotram
		int histMax[ 3 ] = {0};
		int histLoc[ 3 ] = {0};
		for( int p = 0; p < 3; p ++ ) {
			for( int h = 0; h < SEG_HIST + 1; h ++ ) {
				if( hist[ p ][ h ] > histMax[ p ] ) {
					histMax[ p ] = hist[ p ][ h ];
					histLoc[ p ] = h;
				}
			}
		}
		// save paramter
		curS.a = ( histLoc[ 0 ] + 0.5 ) * itv[ 0 ] + minParam[ 0 ];
		curS.b = ( histLoc[ 1 ] + 0.5 ) * itv[ 1 ] + minParam[ 1 ];
		curS.c = ( histLoc[ 2 ] + 0.5 ) * itv[ 2 ] + minParam[ 2 ];
	}
	void histRefineOneSeg( MySegment& curS, Mat& lDis, int* lValid )
	{
		int hei = lDis.rows;
		int wid = lDis.cols;
		for( int i = 0; i < curS.xIdx.size(); i ++ ) {
			// only invalid pixels need to refine
			int curY = curS.yIdx[ i ];
			int curX = curS.xIdx[ i ];
			if( !lValid[ curY * wid + curX ] ) {
				int curD = ( int )( curS.a * curX + curS.b * curY + curS.c );
				lDis.at<uchar>( curY, curX ) = curD;
			}
		}
	}
	void histPlaneFit( Mat& lDis, Mat& rDis, int* lValid, int* rValid, MySegment*& mySeg )
	{
		// compute disparity gradient
		Mat lDGrdX, lDGrdY;
		Mat tmp;
		lDis.convertTo( tmp, CV_32F );
		// X Gradient
		// sobel size must be 1
		Sobel( tmp, lDGrdX, CV_64F, 1, 0 );
		Sobel( tmp, lDGrdY, CV_64F, 0, 1 );

		int hei = lDis.rows;
		int wid = lDis.cols;
		MySegment* pSeg = mySeg;
		int prcSegCnt = 0;
		for( int y = 0;y < hei; y ++ ) {
			for( int x = 0; x < wid; x ++ ) {
				// process each segment
				// segment must > 20 pixels;
				if( (*pSeg).xIdx.size() > 1000 ) {
					printf( "Seg %d ", prcSegCnt );
					prcSegCnt ++;
					// d( x, y ) = ax + by + c
					MySegment curS = *pSeg;
					int valCnt = 0;
					for( int i = 0; i < curS.xIdx.size(); i ++ ) {
						// only valid pixel can be used
						int curY = curS.yIdx[ i ];
						int curX = curS.xIdx[ i ];
						if( lValid[ curY * wid + curX ] ) {
							valCnt ++;
						}
					}
					if( valCnt >= 200 ) {
						// only handle regions with large valid count
						histFitOneSeg( curS, lDis, lDGrdX, lDGrdY, lValid );
						// refine current segment
						histRefineOneSeg( curS, lDis, lValid );
					}
				}
				pSeg ++;
			}
		}
	}
	void lsFitOneSeg( MySegment& curS, const Mat& curDis, int* curValid, const int valCnt )
	{
		int hei = curDis.rows;
		int wid = curDis.cols;
		Mat A = Mat::zeros( valCnt, 3, CV_64F );
		Mat Y = Mat::zeros( valCnt, 1, CV_64F );
		Mat X = Mat::zeros( 3, 1, CV_64F );
		Mat S = Mat::zeros( valCnt, 1, CV_64F ); 
		int v = 0;
		for( int i = 0; i < curS.xIdx.size(); i ++ ) {
			// only valid pixel can be used
			int curY = curS.yIdx[ i ];
			int curX = curS.xIdx[ i ];
			if( curValid[ curY * wid + curX ] ) {
				A.at<double>( v, 0 ) = curX;
				A.at<double>( v, 1 ) = curY;
				A.at<double>( v, 2 ) = 1.0;
				Y.at<double>( v, 0 ) = ( double)( curDis.at<uchar>( curY, curX ) );
				v ++;
			}
		}
		// itetrative L1 minimization
		// min( || Y - AX - S ||_2 + lammbda * || S ||_1 )
		const double lambda = 1;
		const int ITER_NUM = 10;
		for( int i = 0 ; i < ITER_NUM; i ++ ) {
			solve( A, Y - S, X, DECOMP_QR );
			S =  Y - A * X;
			for( int v = 0; v < valCnt; v ++ ) {
				double tmpS = S.at<double>( v, 0 );
				double signS = 0;
				if( tmpS > 0 ) {
					signS = 1;
				} else if( tmpS == 0 ) {
					signS = 0;
				} else {
					signS = - 1;
				}
				double tmp = ( fabs( tmpS ) - lambda );
				S.at<double>( v, 0 ) = signS * ( tmp > 0 ? tmp : 0 );
			}
#ifdef _DEBUG
			printf( "\n\t\t iter:%d\n", i );
			PrintMat<double>( X );
#endif
		}
		
		// save paramter
		curS.a = X.at<double>( 0, 0 );
		curS.b = X.at<double>( 1, 0 );
		curS.c = X.at<double>( 2, 0 );
	}
	void lsRefineOneSeg( MySegment& curS, Mat& curDis, int* curValid )
	{
		int hei = curDis.rows;
		int wid = curDis.cols;
		for( int i = 0; i < curS.xIdx.size(); i ++ ) {
			// only invalid pixels need to refine
			int curY = curS.yIdx[ i ];
			int curX = curS.xIdx[ i ];
			// if( !curValid[ curY * wid + curX ] ) {
				int curD = ( int )( curS.a * curX + curS.b * curY + curS.c );
				curDis.at<uchar>( curY, curX ) = curD;
			//}
		}
	}
	void lsPlaneFit( Mat& lDis, Mat& rDis, int* lValid, int* rValid, MySegment*& lMySeg, MySegment*& rMySeg )
	{
		int hei = lDis.rows;
		int wid = lDis.cols;
		//
		// left plane fit
		//
		MySegment* pSeg = lMySeg;
		int prcSegCnt = 0;
		for( int y = 0;y < hei; y ++ ) {
			for( int x = 0; x < wid; x ++ ) {
				// process each segment
				// segment must > 20 pixels;
				if( (*pSeg).xIdx.size() > MIN_FIT_SZIE ) {
					printf( "Seg %d ", prcSegCnt );
					prcSegCnt ++;
					// d( x, y ) = ax + by + c
					MySegment curS = *pSeg;
					int valCnt = 0;
					for( int i = 0; i < curS.xIdx.size(); i ++ ) {
						// only valid pixel can be used
						int curY = curS.yIdx[ i ];
						int curX = curS.xIdx[ i ];
#ifdef _DEBUG
						if( curY == 352 && curX == 745 ) {
							printf( "haha" );
						}
#endif
						if( lValid[ curY * wid + curX ] ) {
							valCnt ++;
						}
					}
					int totalSize = (*pSeg).xIdx.size();
					int valFlag = 0;
					if( totalSize > DIFF_FIT_CUT ) {
						valFlag = totalSize * SMALL_PERC;
					} else {
						valFlag = totalSize * BIG_PERC;
					}
					if( valCnt >= valFlag ) {
						// only handle regions with large valid count
						lsFitOneSeg( curS, lDis, lValid, valCnt );
						// refine current segment
						lsRefineOneSeg( curS, lDis, lValid );
					}
				}
				pSeg ++;
			}
		}
		//
		// right plane fit
		//
		pSeg = rMySeg;
		prcSegCnt = 0;
		for( int y = 0;y < hei; y ++ ) {
			for( int x = 0; x < wid; x ++ ) {
				// process each segment
				// segment must > 20 pixels;
				if( (*pSeg).xIdx.size() > MIN_FIT_SZIE ) {
					printf( "Seg %d ", prcSegCnt );
					prcSegCnt ++;
					// d( x, y ) = ax + by + c
					MySegment curS = *pSeg;
					int valCnt = 0;
					for( int i = 0; i < curS.xIdx.size(); i ++ ) {
						// only valid pixel can be used
						int curY = curS.yIdx[ i ];
						int curX = curS.xIdx[ i ];
#ifdef _DEBUG
						if( curY == 352 && curX == 745 ) {
							printf( "haha" );
						}
#endif
						if( rValid[ curY * wid + curX ] ) {
							valCnt ++;
						}
					}
					int totalSize = (*pSeg).xIdx.size();
					int valFlag = 0;
					if( totalSize > DIFF_FIT_CUT ) {
						valFlag = totalSize * SMALL_PERC;
					} else {
						valFlag = totalSize * BIG_PERC;
					}
					if( valCnt >= valFlag ) {
						// only handle regions with large valid count
						lsFitOneSeg( curS, rDis, rValid, valCnt );
						// refine current segment
						lsRefineOneSeg( curS, rDis, rValid );
					}
				}
				pSeg ++;
			}
		}
	}
}


void SGPP::postProcess( const Mat& lImg, const Mat& rImg, const int maxDis, const int disSc, Mat& lDis, Mat& rDis,
	Mat& lSeg, Mat& lChk )
{
	// color image should be 3x3 median filtered
	// according to weightedMedianMatlab.m from CVPR11
	Mat lFloat, rFloat;
	Mat lUchar, rUchar;
	lImg.convertTo( lUchar, CV_8U,  255 );
	rImg.convertTo( rUchar, CV_8U,  255 );
	lImg.convertTo( lFloat, CV_32F );
	rImg.convertTo( rFloat, CV_32F );
	int hei = lDis.rows;
	int wid = lDis.cols;
	int imgSize = hei * wid;
	int* lValid = new int[ imgSize ];
	int* rValid = new int[ imgSize ];
	


	// qualise hist of lUchar
	Mat lGray;
	cvtColor( lUchar, lGray, CV_RGB2GRAY );
	equalizeHist( lGray, lGray );
	cvtColor( lGray, lUchar, CV_GRAY2RGB );
	Mat rGray;
	cvtColor( rUchar, rGray, CV_RGB2GRAY );
	equalizeHist( rGray, rGray );
	cvtColor( rGray, rUchar, CV_GRAY2RGB );
	//
	// Graph Segmentation
	//
	image<rgb>* input = new image<rgb>( wid, hei );
	// convert mat to image
	SGPP_FUNC::matToImage( lUchar, input );
	// sgement image
	printf( "\n\t\tSegmentation..." );
	int num_ccs;
	MySegment* lMySeg = new MySegment[ wid * hei ];
	image<rgb> *seg = segment_image(input, SEG_SIGMA, SEG_K, 
		SEG_MIN, &num_ccs, lMySeg ); 
	// get segment image
	SGPP_FUNC::imageToMat( seg, lSeg );
	MySegment* rMySeg = new MySegment[ wid * hei ];
	// convert mat to image
	SGPP_FUNC::matToImage( rUchar, input );
	seg = segment_image(input, SEG_SIGMA, SEG_K, 
		SEG_MIN, &num_ccs, rMySeg ); 
#ifdef _DEBUG
	savePPM(seg, "seg.ppm" );
#endif
	// for( int iter = 0; iter < 3; iter ++ ) {
		// weight median refinement
		printf( "\n\t\tWeight median refinement..." );
		SGPP_FUNC::lrCheck( lDis, rDis, lValid, rValid, disSc );
		SGPP_FUNC::fillInv( lDis, rDis, lValid, rValid );
		SGPP_FUNC::wgtMedian( lFloat, rFloat, lDis, rDis, lValid, rValid, maxDis, disSc );
		//SGPP_FUNC::lrCheck( lDis, rDis, lValid, rValid, disSc );

#ifdef _DEBUG
		imwrite( "before.png", lDis );
#endif
		// plane-fit to refine disparity
		printf( "\n\t\tPlane-fit to refine..." );
		SGPP_FUNC::lsPlaneFit( lDis, rDis, lValid, rValid, lMySeg, rMySeg );


	// }

	// get check image
	SGPP_FUNC::lrCheck( lDis, rDis, lValid, rValid, disSc );
	SGPP_FUNC::saveChk( hei, wid, lValid, rValid, lChk );

	delete input;
	delete [] lMySeg;
	delete [] rMySeg;
	delete [] lValid;
	delete [] rValid;
}
