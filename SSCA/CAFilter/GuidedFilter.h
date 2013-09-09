#pragma once
#include "..\CommFunc.h"
// to use fast inverse method
#define FAST_INV

// cum sum like cumsum in matlab
Mat CumSum( const Mat& src, const int d );

//  %   BOXFILTER   O(1) time box filtering using cumulative sum
//	%
//	%   - Definition imDst(x, y)=sum(sum(imSrc(x-r:x+r,y-r:y+r)));
//  %   - Running time independent of r; 
//  %   - Equivalent to the function: colfilt(imSrc, [2*r+1, 2*r+1], 'sliding', @sum);
//  %   - But much faster.
Mat BoxFilter( const Mat& imSrc, const int r = 9 );
//  %   GUIDEDFILTER   O(1) time implementation of guided filter.
//	%
//	%   - guidance image: I (should be a gray-scale/single channel image)
//	%   - filtering input image: p (should be a gray-scale/single channel image)
//	%   - local window radius: r
//	%   - regularization parameter: eps


Mat GuidedFilter( const Mat& I, const Mat& p, const int r = 9, const float eps = 0.0001 );
