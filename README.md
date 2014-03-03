Cross-Scale Cost Aggregation for Stereo Matching (CVPR 2014)
================

## Compilation
### Windows
The code is a Visual Studio 2010 project on Windows x64 platform. To build the project, you need to configure [OpenCV](http://opencv.org/) on your own PC. (version 2.4.6, however, other versions are acceptable by modifying [CommFunc.h](/SSCA/CommFunc.h)).
### Other Platforms
The code requires no platform-dependent libraries. Thus, it is easy to compile it on other platforms with OpenCV.

## Usage
Run the program with the following paramters:
`Usage: [CC_METHOD] [CA_METHOD] [PP_METHOD] [C_ALPHA] [lImg] [rImg] [lDis] [maxDis] [disSc]`
- `[CC_METHOD]` -- cost computation methods, currently support:
  -  `GRD` -- [intensity + gradient](#GF)
  -  `CEN` -- [Census Transform](#CT)
  -  `CG`  -- Census + gradient
- `[CA_METHOD]` -- cost aggregation methods, currently support:
  - `GF` -- [guided image filter](#GF)
  - `BF` -- [bilateral filter](#BF)
  - `BOX` -- box filter
  - `NL` -- [non-local cost aggregation](#NL)
  - `ST` -- [segment-tree cost aggregation](#ST)
- `[PP_METHOD]` -- post processing methods, currently support:
  - `WM` -- [weighted median filtering](#GF)
  - `SG` -- segment based (experimental)
- `[C_ALPHA]` -- regularization paramter, i.e. `$\lambda$` in the paper.
- `[lImg]` -- input left color image file name. (all formats supported by OpenCV)
- `[rImg]` -- input right color image file name.
- `[lDis]` -- output left disparity map file name.
- `[maxDis]` -- maximum disparity range, e.g. `60` for Middlebury and `256` for KITTI dataets.
- `[disSc]` -- scale disparity, e.g. `4` for Middlebury and `1` for KITTI datasets.

**Hint**: to enable post-processing, you must uncomment `// #define COMPUTE_RIGHT` in [CommFunc.h](/SSCA/CommFunc.h) to allow computing right disparity map.

## Citation
Citation is very important for researchers. If you find this code useful, please cite:
```
@inproceedings{CrossScaleStereo,
        author    = {Kang Zhang and Yuqiang Fang  and Dongbo Min and Lifeng Sun and Shiqiang Yang  and Shuicheng Yan and Qi Tian},
        title     = {Cross-Scale Cost Aggregation for Stereo Matching},
        booktitle = {CVPR},
        year     = {2014}
}
```
Since some cost aggregation methods ([GF](#GF), [NL](#NL), [ST](#ST)) are built uppon other papers' code, you also need to cite corresponding papers as listed below.

## Reference
<a name="CT">[CT]</a>: R. Zabih and J. Woodfill. Non-parametric local transforms for computing visual correspondence. In ECCV, 1994

<a name="GF">[GF]</a>: C. Rhemann, A. Hosni, M. Bleyer, C. Rother, and M. Gelautz. Fast cost-volume filtering for visual correspondence and beyond. In CVPR, 2011

<a name="ST">[ST]</a>: X. Mei, X. Sun, W. Dong, H. Wang, and X. Zhang. Segment-tree based cost aggregation for stereo matching. In CVPR, 2013

<a name="BF">[BF]</a>: K.-J. Yoon and I. S. Kweon. Adaptive support-weight approach for correspondence search. TPAMI, 2006

<a name="NL">[NL]</a>: Q. Yang. A non-local cost aggregation method for stereo matching. In CVPR, 2012
