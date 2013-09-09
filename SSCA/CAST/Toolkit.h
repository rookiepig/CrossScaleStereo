/*
This project implements the methods in the following paper. Please cite this paper, depending on the use.

Xing Mei, Xun Sun, Weiming Dong, Haitao Wang and Xiaopeng Zhang. Segment-Tree based Cost Aggregation for Stereo Matching, in CVPR 2013.

The code is written by Yan Kong, <kongyanwork@gmail.com>, 2013.

LICENSE
Copyright (C) 2012-2013 by Yan Kong
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __TOOLKIT_H__
#define __TOOLKIT_H__
#include <cassert>
#include <opencv2/core/core.hpp>

#define TAU 1200
#define SIGMA_ONE (0.08f)
template<typename _Tp, int cn>
class KIdx_ {
public:
    KIdx_(_Tp *data, int c0) {
        assert(cn == 1);
        p = data;
        sum = dim[0] = c0;
    }

    KIdx_(_Tp *data, int c0, int c1) {
        assert(cn == 2);
        p = data;
        dim[0] = c0; dim[1] = c1;
		sum = dim[0] * dim[1];
    }

    KIdx_(_Tp *data, int c0, int c1, int c2) {
        assert(cn == 3);
        p = data;
        dim[0] = c0; dim[1] = c1; dim[2] = c2;
		sum = dim[0] * dim[1] * dim[2];
    }

    KIdx_(_Tp *data, int c0, int c1, int c2, int c3) {
        assert(cn == 4);
        p = data;
        dim[0] = c0; dim[1] = c1; dim[2] = c2; dim[3] = c3;
		sum = dim[0] * dim[1] * dim[2] * dim[3];
    }

    _Tp &operator()(int x) const {
        assert(x < sum);
        return p[x];
    }

    _Tp &operator()(int y, int x) const {
        assert(cn == 2 && y < dim[0] && x < dim[1]);
        return p[y * dim[1] + x];
    }

    _Tp &operator()(int z, int y, int x) const {
        assert(cn == 3 && z < dim[0] && y < dim[1] && x < dim[2]);
        return p[z * dim[2] * dim[1] + y * dim[2] + x];
    }

    _Tp &operator()(int w, int z, int y, int x) const {
        assert(cn == 4 && w < dim[0] && z < dim[1] && y < dim[2] && x < dim[3]);
        return p[w * dim[3] * dim[2] * dim[1] + z * dim[3] * dim[2] + y * dim[3] + x];
    }

	int index(int x) const {
        assert(cn == 1 && x < dim[0]);
        return x;
    }

    int index(int y, int x) const {
        assert(cn == 2 && y < dim[0] && x < dim[1]);
        return y * dim[1] + x;
    }

    int index(int z, int y, int x) const {
        assert(cn == 3 && z < dim[0] && y < dim[1] && x < dim[2]);
        return z * dim[2] * dim[1] + y * dim[2] + x;
    }

    int index(int w, int z, int y, int x) const {
        assert(cn == 4 && w < dim[0] && z < dim[1] && y < dim[2] && x < dim[3]);
        return w * dim[3] * dim[2] * dim[1] + z * dim[3] * dim[2] + y * dim[3] + x;
    }

    void set_data(_Tp *data) {
        p = data;
    }

	long long sum;
    int dim[cn];
    _Tp *p;
};

void MeanFilter(cv::InputArray iImage_, cv::OutputArray oImage_, int r);


#endif