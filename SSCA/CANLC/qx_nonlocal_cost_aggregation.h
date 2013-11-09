/******************************************************************************************
\Author:	Qingxiong Yang
\Function:	1. Compute matching cost using image color/intensity and gradient,
			2. Compute disparity map using filtering method presented in qx_tree_filter.h
\Reference:	Qingxiong Yang, A Non-Local Cost Aggregation Method for Stereo Matching,
			IEEE Conference on Computer Vision and Pattern Recognition (CVPR) 2012.
*******************************************************************************************/
#ifndef QX_NONLOCAL_COST_AGGREGATION_H
#define QX_NONLOCAL_COST_AGGREGATION_H
#include "qx_tree_filter.h"
#include <opencv2/opencv.hpp>
#include <bitset>
using namespace std;
using namespace cv;
//#define USE_CENCUS
#define CENCUS_WND 9
#define CENCUS_BIT 80

class qx_nonlocal_cost_aggregation
{
public:
    qx_nonlocal_cost_aggregation();
    ~qx_nonlocal_cost_aggregation();
    void clean();
	int init(int h,int w,int nr_plane,
		double sigma_range=0.1,
		double max_color_difference=7,
		double max_gradient_difference=2,
		double weight_on_color=0.11
		);
    int matching_cost(unsigned char***left,unsigned char***right);
    int disparity(unsigned char**disparity,bool use_nonlocal_post_processing=false);
public:
	void matching_cost_from_color_and_gradient(unsigned char***left,unsigned char***right);
	/* my revise */
	// census cost
	void my_matching_cost_from_cencus(unsigned char*** left, unsigned char*** right );
	void compute_gradient(float**gradient,unsigned char***image);
	void compute_filter_weights(unsigned char***texture);
	void filter(float**image_filtered,float**image,bool compute_weight=true);
	void buildLeftVolume();
	void buildRightVolume();
public:
	qx_tree_filter m_tf,m_tf_right;
	int	m_h,m_w,m_nr_plane; double m_max_color_difference,m_max_gradient_difference,m_weight_on_color,m_weight_on_color_inv;
	unsigned char**m_disparity,**m_disparity_right,**m_mask_occlusion,***m_buf_u2;
	double m_table[256],m_sigma_range;
	unsigned char***m_left,***m_right,****m_buf_u3,***m_image_shifted,***m_image_temp;
	float***m_buf_f2,**m_cost_min,**m_cost_temp,**m_cost,**m_gradient_left,**m_gradient_right,**m_gradient_shifted;
	double****m_buf_d3,***m_cost_vol,***m_cost_vol_right,***m_cost_vol_backup,***m_cost_vol_temp;
};
#endif