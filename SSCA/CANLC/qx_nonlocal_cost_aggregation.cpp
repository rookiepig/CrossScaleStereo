/*$Id: qx_nonlocal_cost_aggregation.cpp,v 1.1 2007/02/16 04:11:12 liiton Exp $*/
#include "qx_basic.h"
#include "qx_nonlocal_cost_aggregation.h"
qx_nonlocal_cost_aggregation::qx_nonlocal_cost_aggregation()
{
	m_buf_u2=NULL;
	m_buf_u3=NULL;
	m_buf_f2=NULL;
	m_buf_d3=NULL;
}
qx_nonlocal_cost_aggregation::~qx_nonlocal_cost_aggregation()
{
    clean();
}
void qx_nonlocal_cost_aggregation::clean()
{
	qx_freeu_3(m_buf_u2); m_buf_u2=NULL;
	qx_freeu_4(m_buf_u3); m_buf_u3=NULL;
	qx_freef_3(m_buf_f2); m_buf_f2=NULL;
	qx_freed_4(m_buf_d3); m_buf_d3=NULL;
}
int qx_nonlocal_cost_aggregation::init(int h,int w,int nr_plane,double sigma_range,double max_color_difference,double max_gradient_difference,double weight_on_color)
{
	clean();
	m_h=h; m_w=w; m_nr_plane=nr_plane; m_sigma_range=sigma_range;
	m_max_color_difference=max_color_difference; m_max_gradient_difference=max_gradient_difference;
	m_weight_on_color=weight_on_color;
	m_weight_on_color_inv=1-m_weight_on_color;

	m_buf_f2=qx_allocf_3(6,m_h,m_w);
	m_buf_d3=qx_allocd_4(4,m_h,m_w,m_nr_plane);
	m_cost_vol=m_buf_d3[0];
	m_cost_vol_backup=m_buf_d3[1];
	m_cost_vol_temp=m_buf_d3[2];
	m_cost_vol_right=m_buf_d3[3];
	m_cost_min=m_buf_f2[0];
	m_cost_temp=m_buf_f2[1];
	m_cost=m_buf_f2[2];
	m_gradient_shifted=m_buf_f2[3];
	m_gradient_left=m_buf_f2[4];
	m_gradient_right=m_buf_f2[5];
	for(int y=0;y<m_h;y++) for(int x=0;x<m_w;x++) m_cost_temp[y][x]=QX_DEF_FLOAT_MAX;
	m_buf_u3=qx_allocu_4(2,m_h,m_w,3);
	m_image_shifted=m_buf_u3[0];
	m_image_temp=m_buf_u3[1];
	m_buf_u2=qx_allocu_3(3,m_h,m_w);
	m_disparity=m_buf_u2[0];
	m_disparity_right=m_buf_u2[1];
	m_mask_occlusion=m_buf_u2[2];


	for(int i=0;i<256;i++) m_table[i]=exp(-double(i)/(m_sigma_range*255));
	m_tf.init(m_h,m_w,3,m_sigma_range,4);
	m_tf_right.init(m_h,m_w,3,m_sigma_range,4);
    return(0);
}
int qx_nonlocal_cost_aggregation::matching_cost(unsigned char***left,unsigned char***right)
{
	m_left=left; m_right=right; 
#ifdef USE_CENCUS
	my_matching_cost_from_cencus(left,right);
#else
	matching_cost_from_color_and_gradient(left,right);
#endif
	image_copy(m_cost_vol_backup,m_cost_vol,m_h,m_w,m_nr_plane);
	qx_stereo_flip_corr_vol(m_cost_vol_right,m_cost_vol,m_h,m_w,m_nr_plane);
	
	m_tf.build_tree(m_left[0][0]);
	m_tf_right.build_tree(m_right[0][0]);
    return(0);
}
int qx_nonlocal_cost_aggregation::disparity(unsigned char**disparity,bool use_nonlocal_post_processing)
{
	int radius=2;
	image_copy(m_cost_vol,m_cost_vol_backup,m_h,m_w,m_nr_plane);
	m_tf.filter(m_cost_vol[0][0],m_cost_vol_temp[0][0],m_nr_plane);
	depth_best_cost(m_disparity,m_cost_vol,m_h,m_w,m_nr_plane);
	ctmf(m_disparity[0],disparity[0],m_w,m_h,m_w,m_w,radius,1,m_h*m_w);
	//image_copy(disparity,m_disparity,m_h,m_w);
	
	if(use_nonlocal_post_processing)
	{
		image_copy(m_cost_vol,m_cost_vol_right,m_h,m_w,m_nr_plane);
		m_tf_right.filter(m_cost_vol[0][0],m_cost_vol_temp[0][0],m_nr_plane);
		depth_best_cost(m_disparity,m_cost_vol,m_h,m_w,m_nr_plane);
		ctmf(m_disparity[0],m_disparity_right[0],m_w,m_h,m_w,m_w,radius,1,m_h*m_w);
		//image_display(m_disparity_right,m_h,m_w);
		//image_display(disparity,m_h,m_w);

		//qx_occlusion_solver_left_right(disparity,m_disparity_right,m_h,m_w,m_nr_plane,false);
		qx_detect_occlusion_left_right(m_mask_occlusion,disparity,m_disparity_right,m_h,m_w,m_nr_plane);
		image_zero(m_cost_vol,m_h,m_w,m_nr_plane);
		//int th=int(0.1*m_nr_plane+0.5);
		for(int y=0;y<m_h;y++) for(int x=0;x<m_w;x++) if(!m_mask_occlusion[y][x])
		{
			for(int d=0;d<m_nr_plane;d++) 
				//m_cost_vol[y][x][d]=min(abs(disparity[y][x]-d),th);
				//m_cost_vol[y][x][d]=(disparity[y][x]-d)*(disparity[y][x]-d);
				m_cost_vol[y][x][d]=abs(disparity[y][x]-d);
		}
		m_tf.update_table(m_sigma_range/2);
		m_tf.filter(m_cost_vol[0][0],m_cost_vol_temp[0][0],m_nr_plane);
		//depth_best_cost(disparity,m_cost_vol,m_h,m_w,m_nr_plane);
		depth_best_cost(m_disparity,m_cost_vol,m_h,m_w,m_nr_plane);
		ctmf(m_disparity[0],disparity[0],m_w,m_h,m_w,m_w,radius,1,m_h*m_w);

	}
    return(0);
}
void qx_nonlocal_cost_aggregation::my_matching_cost_from_cencus(unsigned char*** left, unsigned char*** right )
{
	image_zero(m_cost_vol,m_h,m_w,m_nr_plane);
	// copy data to mat
	Mat lTmp = Mat::zeros( m_h, m_w, CV_8UC3 );
	Mat rTmp = Mat::zeros( m_h, m_w, CV_8UC3 );
	uchar* lData = left[ 0 ][ 0 ];
	uchar* rData = right[ 0 ][ 0 ];
	for( int y = 0; y < m_h; y ++ ) {
		uchar* lTmpData = ( uchar* ) ( lTmp.ptr<uchar>( y ) );
		uchar* rTmpData = ( uchar* ) ( rTmp.ptr<uchar>( y ) );
		for( int x = 0; x < m_w; x ++ ) {
			lTmpData[ 0 ] = lData[ 0 ];
			lTmpData[ 1 ] = lData[ 1 ];
			lTmpData[ 2 ] = lData[ 2 ];
			rTmpData[ 0 ] = rData[ 0 ];
			rTmpData[ 1 ] = rData[ 1 ];
			rTmpData[ 2 ] = rData[ 2 ];
			lTmpData += 3;
			rTmpData += 3;
			lData    += 3;
			rData    += 3;
		}
	}
	// cvt color to gray
	Mat lGray, rGray;
	cvtColor( lTmp, lGray, CV_RGB2GRAY );
	cvtColor( rTmp, rGray, CV_RGB2GRAY );
	// prepare binary code 
	int H_WD = CENCUS_WND / 2;
	bitset<CENCUS_BIT>* lCode = new bitset<CENCUS_BIT>[ m_w * m_h ];
	bitset<CENCUS_BIT>* rCode = new bitset<CENCUS_BIT>[ m_w * m_h ];
	bitset<CENCUS_BIT>* pLCode = lCode;
	bitset<CENCUS_BIT>* pRCode = rCode;
	for( int y = 0; y < m_h; y ++ ) {
		uchar* pLData = ( uchar* ) ( lGray.ptr<uchar>( y ) );
		uchar* pRData = ( uchar* ) ( rGray.ptr<uchar>( y ) );
		for( int x = 0; x < m_w; x ++ ) {
			int bitCnt = 0;
			for( int wy = - H_WD; wy <= H_WD; wy ++ ) {
				int qy = ( y + wy + m_h ) % m_h;
				uchar* qLData = ( uchar* ) ( lGray.ptr<uchar>( qy ) );
				uchar* qRData = ( uchar* ) ( rGray.ptr<uchar>( qy ) );
				for( int wx = - H_WD; wx <= H_WD; wx ++ ) {
					if( wy != 0 || wx != 0 ) {
						int qx = ( x + wx + m_w ) % m_w;
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
	bitset<CENCUS_BIT> lB;
	bitset<CENCUS_BIT> rB;
	pLCode = lCode;
	for( int y = 0; y < m_h; y ++ ) {
		int index = y * m_w;
		for( int x = 0; x < m_w; x ++ ) {
			lB = *pLCode;
			for( int d = 0; d < m_nr_plane; d ++ ) {
				m_cost_vol[y][x][d] = CENCUS_BIT;
				if( x - d >= 0 ) {
					rB = rCode[ index + x - d ];
					m_cost_vol[y][x][d] = ( lB ^ rB ).count();
				}

			}
			pLCode ++;
		}
	}
	delete [] lCode;
	delete [] rCode;

}
void qx_nonlocal_cost_aggregation::matching_cost_from_color_and_gradient(unsigned char ***left,unsigned char ***right)
{
	image_zero(m_cost_vol,m_h,m_w,m_nr_plane);
	compute_gradient(m_gradient_left,left);
	compute_gradient(m_gradient_right,right);

	for(int i=0;i<m_nr_plane;i++)
	{
		for(int y=0;y<m_h;y++) //shift the right image by i pixels
		{
			image_copy((&m_image_shifted[y][i]),right[y],m_w-i,3);
			memcpy(&(m_gradient_shifted[y][i]),m_gradient_right[y],sizeof(float)*(m_w-i));
			for(int x=0;x<i;x++) 
			{
				qx_memcpy_u3(m_image_shifted[y][x],right[y][0]);//m_cost_on_border_occlusion;
				m_gradient_shifted[y][x]=m_gradient_right[y][0];//m_cost_on_border_occlusion;
			}
		}
		//for(int y=0;y<m_h;y++) for(int x=0;x<m_w-1;x++) 
		for(int y=0;y<m_h;y++) for(int x=0;x<(m_w);x++) 
		{
			double cost=0;
			for(int c=0;c<3;c++) cost+=abs(left[y][x][c]-m_image_shifted[y][x][c]);
			cost=min(cost/3,m_max_color_difference);
			double cost_gradient=min((double)abs(m_gradient_left[y][x]-m_gradient_shifted[y][x]),m_max_gradient_difference);
			m_cost_vol[y][x][i]=m_weight_on_color*cost+m_weight_on_color_inv*cost_gradient;
		}
	}
}
void qx_nonlocal_cost_aggregation::compute_gradient(float**gradient,unsigned char***image)
{
	float gray,gray_minus,gray_plus;
	for(int y=0;y<m_h;y++)
	{
		gray_minus=rgb_2_gray(image[y][0]);
		gray=gray_plus=rgb_2_gray(image[y][1]);
		gradient[y][0]=gray_plus-gray_minus+127.5;
		for(int x=1;x<m_w-1;x++)
		{
			gray_plus=rgb_2_gray(image[y][x+1]);
			gradient[y][x]=0.5*(gray_plus-gray_minus)+127.5;
			gray_minus=gray;
			gray=gray_plus;
		}
		gradient[y][m_w-1]=gray_plus-gray_minus+127.5;
	}
}

void qx_nonlocal_cost_aggregation::buildLeftVolume()
{
	m_tf.filter(m_cost_vol[0][0],m_cost_vol_temp[0][0],m_nr_plane);
}

void qx_nonlocal_cost_aggregation::buildRightVolume()
{
	m_tf_right.filter(m_cost_vol_right[0][0],m_cost_vol_temp[0][0],m_nr_plane);

}
