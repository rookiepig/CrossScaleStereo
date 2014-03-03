///////////////////////////////////////////////////////
// File: CommonFunc
// Desc: Common function + Hearder files
//
// Author: Zhang Kang
// Date: 2013/09/06
///////////////////////////////////////////////////////
#pragma  once
#define DOUBLE_MAX 1e10
// #define COMPUTE_RIGHT

#include<opencv2/opencv.hpp>
#include<string>
#include<iostream>
#include<bitset>
using namespace std;
using namespace cv;

//
// Opencv Lib 2.4.6
//
#ifdef _DEBUG
#pragma comment( lib, "opencv_calib3d246d.lib" )
#pragma comment( lib, "opencv_contrib246d.lib" )
#pragma comment( lib, "opencv_core246d.lib" )
#pragma comment( lib, "opencv_features2d246d.lib" )
#pragma comment( lib, "opencv_flann246d.lib" )
#pragma comment( lib, "opencv_gpu246d.lib" )
#pragma comment( lib, "opencv_highgui246d.lib" )
#pragma comment( lib, "opencv_imgproc246d.lib" )
#pragma comment( lib, "opencv_legacy246d.lib" )
#pragma comment( lib, "opencv_ml246d.lib" )
#pragma comment( lib, "opencv_nonfree246d.lib" )
#pragma comment( lib, "opencv_objdetect246d.lib" )
#pragma comment( lib, "opencv_photo246d.lib" )
#pragma comment( lib, "opencv_stitching246d.lib" )
#pragma comment( lib, "opencv_superres246d.lib" )
#pragma comment( lib, "opencv_ts246d.lib" )
#pragma comment( lib, "opencv_video246d.lib" )
#pragma comment( lib, "opencv_videostab246d.lib" )
#else
#pragma comment( lib, "opencv_calib3d246.lib" )
#pragma comment( lib, "opencv_contrib246.lib" )
#pragma comment( lib, "opencv_core246.lib" )
#pragma comment( lib, "opencv_features2d246.lib" )
#pragma comment( lib, "opencv_flann246.lib" )
#pragma comment( lib, "opencv_gpu246.lib" )
#pragma comment( lib, "opencv_highgui246.lib" )
#pragma comment( lib, "opencv_imgproc246.lib" )
#pragma comment( lib, "opencv_legacy246.lib" )
#pragma comment( lib, "opencv_ml246.lib" )
#pragma comment( lib, "opencv_nonfree246.lib" )
#pragma comment( lib, "opencv_objdetect246.lib" )
#pragma comment( lib, "opencv_photo246.lib" )
#pragma comment( lib, "opencv_stitching246.lib" )
#pragma comment( lib, "opencv_superres246.lib" )
#pragma comment( lib, "opencv_ts246.lib" )
#pragma comment( lib, "opencv_video246.lib" )
#pragma comment( lib, "opencv_videostab246.lib" )
#endif

// output matrix
template<class T>
void PrintMat( const Mat& mat )
{
	int rows = mat.rows;
	int cols = mat.cols;
	printf( "\n%d x %d Matrix\n", rows, cols );
	for( int r = 0; r < rows; r ++ ) {
		for( int c = 0; c < cols; c ++  ) {
			cout << mat.at<T>( r, c ) << "\t";
		}
		printf( "\n" );
	}
	printf( "\n" );
}
