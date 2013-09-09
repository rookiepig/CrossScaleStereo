#pragma once
#include "..\CommFunc.h"

// bilateral filter
Mat BilateralFilter( const Mat& I, const Mat& p, const int wndSZ = 9, double sig_sp = 4.5, const double sig_clr = 0.03 );
