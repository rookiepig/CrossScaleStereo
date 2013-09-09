#include "qx_basic.h"

#include <windows.h>//The win32 API library 
/*timer*/
void qx_timer::start()
{
	//m_begin=clock();
	
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
        cout << "QueryPerformanceFrequency failed!\n";

    m_pc_frequency = double(li.QuadPart);///1000.0;

    QueryPerformanceCounter(&li);
    m_counter_start = li.QuadPart;
}
double qx_timer::stop()
{
	//m_end=clock(); return ( double(m_end-m_begin)/CLOCKS_PER_SEC );
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-m_counter_start)/m_pc_frequency;
}
void qx_timer::time_display(char *disp,int nr_frame){ printf("Running time (%s) is: %5.5f Seconds.\n",disp,stop()/nr_frame);}
void qx_timer::fps_display(char *disp,int nr_frame){ printf("Running time (%s) is: %5.5f frame per second.\n",disp,(double)nr_frame/stop());}
void boxcar_sliding_window_x(double *out,double *in,int h,int w,int radius)
{
    double scale = 1.0f / (2*radius+1);    
    for (int y = 0; y < h; y++) {    
        double t;
        // do left edge
        t = in[y*w] * radius;
        for (int x = 0; x < radius+1; x++) {
            t += in[y*w+x];
        }
        out[y*w] = t * scale;
        for(int x = 1; x < radius+1; x++) {
            int c = y*w+x;
            t += in[c+radius];
            t -= in[y*w];
            out[c] = t * scale;
        }        
        // main loop
        for(int x = radius+1; x < w-radius; x++) {
            int c = y*w+x;
            t += in[c+radius];
            t -= in[c-radius-1];
            out[c] = t * scale;
        }
        // do right edge
        for (int x = w-radius; x < w; x++) {
            int c = y*w+x;
            t += in[(y*w)+w-1];
            t -= in[c-radius-1];
            out[c] = t * scale;
        }
        
    }
}
void boxcar_sliding_window_y(double *out,double *in,int h,int w,int radius)
{
    double scale = 1.0f / (2*radius+1);    
    for (int x = 0; x < w; x++) 
	{    
        double t;
        // do left edge
        t = in[x] * radius;
        for (int y = 0; y < radius+1; y++) {
            t += in[y*w+x];
        }
        out[x] = t * scale;
        for(int y = 1; y < radius+1; y++) {
            int c = y*w+x;
            t += in[c+radius*w];
            t -= in[x];
            out[c] = t * scale;
        }        
        // main loop
        for(int y = radius+1; y < h-radius; y++) {
            int c = y*w+x;
            t += in[c+radius*w];
            t -= in[c-(radius*w)-w];
            out[c] = t * scale;
        }
        // do right edge
        for (int y = h-radius; y < h; y++) {
            int c = y*w+x;
            t += in[(h-1)*w+x];
            t -= in[c-(radius*w)-w];
            out[c] = t * scale;
        }        
    }
}
void boxcar_sliding_window(double **out,double **in,double **temp,int h,int w,int radius)
{
    boxcar_sliding_window_x(temp[0],in[0],h,w,radius);
    boxcar_sliding_window_y(out[0],temp[0],h,w,radius);
}

void boxcar_sliding_window_x(float *out,float *in,int h,int w,int radius)
{
    float scale = 1.0f / (2*radius+1);    
    for (int y = 0; y < h; y++) {    
        float t;
        // do left edge
        t = in[y*w] * radius;
        for (int x = 0; x < radius+1; x++) {
            t += in[y*w+x];
        }
        out[y*w] = t * scale;
        for(int x = 1; x < radius+1; x++) {
            int c = y*w+x;
            t += in[c+radius];
            t -= in[y*w];
            out[c] = t * scale;
        }        
        // main loop
        for(int x = radius+1; x < w-radius; x++) {
            int c = y*w+x;
            t += in[c+radius];
            t -= in[c-radius-1];
            out[c] = t * scale;
        }
        // do right edge
        for (int x = w-radius; x < w; x++) {
            int c = y*w+x;
            t += in[(y*w)+w-1];
            t -= in[c-radius-1];
            out[c] = t * scale;
        }
        
    }
}
void boxcar_sliding_window_y(float *out,float *in,int h,int w,int radius)
{
    float scale = 1.0f / (2*radius+1);    
    for (int x = 0; x < w; x++) 
	{    
        float t;
        // do left edge
        t = in[x] * radius;
        for (int y = 0; y < radius+1; y++) {
            t += in[y*w+x];
        }
        out[x] = t * scale;
        for(int y = 1; y < radius+1; y++) {
            int c = y*w+x;
            t += in[c+radius*w];
            t -= in[x];
            out[c] = t * scale;
        }        
        // main loop
        for(int y = radius+1; y < h-radius; y++) {
            int c = y*w+x;
            t += in[c+radius*w];
            t -= in[c-(radius*w)-w];
            out[c] = t * scale;
        }
        // do right edge
        for (int y = h-radius; y < h; y++) {
            int c = y*w+x;
            t += in[(h-1)*w+x];
            t -= in[c-(radius*w)-w];
            out[c] = t * scale;
        }
    }
}
void boxcar_sliding_window(float**out,float**in,float**temp,int h,int w,int radius)
{
	int min_hw=min(h,w);
	if(radius>=min_hw)
	{
		double dsum=0;
		float*in_=in[0];
		for(int y=0;y<h;y++) for(int x=0;x<w;x++) dsum+=*in_++;
		dsum/=(h*w);
		float fsum=(float)dsum;
		float*out_=out[0];
		for(int y=0;y<h;y++) for(int x=0;x<w;x++) *out_++=fsum;
	}
	else if(radius>0)
	{
		boxcar_sliding_window_x(temp[0],in[0],h,w,radius);
		boxcar_sliding_window_y(out[0],temp[0],h,w,radius);
	}
	else memcpy(out[0],in[0],sizeof(float)*h*w);
}
void boxcar_sliding_window_x(unsigned char*out,unsigned char*in,int h,int w,int radius)
{
    float scale = 1.0f / (2*radius+1);    
    for (int y = 0; y < h; y++) {    
        float t;
        // do left edge
        t = in[y*w] * radius;
        for (int x = 0; x < radius+1; x++) {
            t += in[y*w+x];
        }
        out[y*w] = unsigned char(t * scale+0.5);
        for(int x = 1; x < radius+1; x++) {
            int c = y*w+x;
            t += in[c+radius];
            t -= in[y*w];
            out[c] = unsigned char(t * scale+0.5);
        }        
        // main loop
        for(int x = radius+1; x < w-radius; x++) {
            int c = y*w+x;
            t += in[c+radius];
            t -= in[c-radius-1];
            out[c] = unsigned char(t * scale+0.5);
        }
        // do right edge
        for (int x = w-radius; x < w; x++) {
            int c = y*w+x;
            t += in[(y*w)+w-1];
            t -= in[c-radius-1];
            out[c] = unsigned char(t * scale+0.5);
        }
        
    }
}
void boxcar_sliding_window_y(unsigned char*out,unsigned char*in,int h,int w,int radius)
{
    float scale = 1.0f / (2*radius+1);    
    for (int x = 0; x < w; x++) 
	{    
        float t;
        // do left edge
        t = in[x] * radius;
        for (int y = 0; y < radius+1; y++) {
            t += in[y*w+x];
        }
        out[x] = unsigned char(t * scale+0.5);
        for(int y = 1; y < radius+1; y++) {
            int c = y*w+x;
            t += in[c+radius*w];
            t -= in[x];
            out[c] = unsigned char(t * scale+0.5);
        }        
        // main loop
        for(int y = radius+1; y < h-radius; y++) {
            int c = y*w+x;
            t += in[c+radius*w];
            t -= in[c-(radius*w)-w];
            out[c] = unsigned char(t * scale+0.5);
        }
        // do right edge
        for (int y = h-radius; y < h; y++) {
            int c = y*w+x;
            t += in[(h-1)*w+x];
            t -= in[c-(radius*w)-w];
            out[c] = unsigned char(t * scale+0.5);
        }
    }
}
void boxcar_sliding_window(unsigned char**out,unsigned char**in,unsigned char**temp,int h,int w,int radius)
{
	int min_hw=min(h,w);
	if(radius<min_hw)
	{
		boxcar_sliding_window_x(temp[0],in[0],h,w,radius);
		boxcar_sliding_window_y(out[0],temp[0],h,w,radius);
	}
	else
	{
		double dsum=0;
		unsigned char*in_=in[0];
		for(int y=0;y<h;y++) for(int x=0;x<w;x++) dsum+=*in_++;
		dsum/=(h*w);
		unsigned char usum=unsigned char(dsum+0.5);
		unsigned char*out_=out[0];
		for(int y=0;y<h;y++) for(int x=0;x<w;x++) *out_++=usum;
	}
}
void gaussian_recursive_x(double **od,double **id, int w, int h, double a0, double a1, double a2, double a3, double b1, double b2, double coefp, double coefn)
{
    double xp = 0.0f;  // previous input
    double yp = 0.0f;  // previous output
    double yb = 0.0f;  // previous output by 2
	for(int y=0;y<h;y++)
	{
		xp = id[y][0]; yb = coefp*xp; yp = yb;
		for (int x = 0; x < w; x++) 
		{
			double xc = id[y][x];
			double yc = a0*xc + a1*xp - b1*yp - b2*yb;
			od[y][x] = yc;
			xp = xc; yb = yp; yp = yc; 
		}
	}
    // reverse pass
    // ensures response is symmetrical
    double xn = 0.f;
    double xa = 0.f;
    double yn = 0.f;
    double ya = 0.f;
	for(int y=0;y<h;y++)
	{
		xn = xa = id[y][w-1]; yn = coefn*xn; ya = yn;
		for (int x = w-1; x >= 0; x--) {
			double xc = id[y][x];
			double yc = a2*xn + a3*xa - b1*yn - b2*ya;
			xa = xn; xn = xc; ya = yn; yn = yc;
			od[y][x] = od[y][x]+ yc;
		}
	}
}
void gaussian_recursive_y(double **od,double **id, int w, int h, double a0, double a1, double a2, double a3, double b1, double b2, double coefp, double coefn)
{
    double xp = 0.0f;  // previous input
    double yp = 0.0f;  // previous output
    double yb = 0.0f;  // previous output by 2
	for (int x = 0; x < w; x++)
	{
		xp = id[0][x]; yb = coefp*xp; yp = yb;
		for(int y=0;y<h;y++)
		{
			double xc = id[y][x];
			double yc = a0*xc + a1*xp - b1*yp - b2*yb;
			od[y][x] = yc;
			xp = xc; yb = yp; yp = yc; 
		}
	}


    // reverse pass
    // ensures response is symmetrical
    double xn = 0.f;
    double xa = 0.f;
    double yn = 0.f;
    double ya = 0.f;
	for (int x = 0; x < w; x++)
	{
		xn = xa = id[h-1][x]; yn = coefn*xn; ya = yn;		
		for(int y=h-1;y>=0;y--)
		{
			double xc = id[y][x];
			double yc = a2*xn + a3*xa - b1*yn - b2*ya;
			xa = xn; xn = xc; ya = yn; yn = yc;
			od[y][x] = od[y][x]+ yc;
		}
	}
}
int gaussian_recursive(double **image,double **temp,double sigma,int order,int h,int w)
{
    const double
        nsigma = sigma < 0.1f ? 0.1f : sigma,
        alpha = 1.695f / nsigma,
        ema = exp(-alpha),
        ema2 = exp(-2*alpha),
        b1 = -2*ema,
        b2 = ema2;
    double a0 = 0, a1 = 0, a2 = 0, a3 = 0, coefp = 0, coefn = 0;
    switch (order) {
    case 0: {
        const double k = (1-ema)*(1-ema)/(1+2*alpha*ema-ema2);
        a0 = k;
        a1 = k*(alpha-1)*ema;
        a2 = k*(alpha+1)*ema;
        a3 = -k*ema2;
    } break;

    case 1: {
        const double k = (1-ema)*(1-ema)/ema;
        a0 = k*ema;
        a1 = a3 = 0;
        a2 = -a0;
    } break;

    case 2: {
        const double
            ea = exp(-alpha),
            k = -(ema2-1)/(2*alpha*ema),
            kn = (-2*(-1+3*ea-3*ea*ea+ea*ea*ea)/(3*ea+1+3*ea*ea+ea*ea*ea));
        a0 = kn;
        a1 = -kn*(1+k*alpha)*ema;
        a2 = kn*(1-k*alpha)*ema;
        a3 = -kn*ema2;
    } break;

    default:
        fprintf(stderr, "gaussianFilter: invalid order parameter!\n");
        return 0;
    }
    coefp = (a0+a1)/(1+b1+b2);
    coefn = (a2+a3)/(1+b1+b2);
	//timer.start();
	gaussian_recursive_x(temp,image,w,h,a0,a1,a2,a3,b1,b2,coefp,coefn);
	//image_display(temp,h,w);
	gaussian_recursive_y(image,temp,w,h,a0,a1,a2,a3,b1,b2,coefp,coefn);
	//timer.fps_display();
	return(0);
}
void gaussian_recursive_x(float **od,float **id, int w, int h, float a0, float a1, float a2, float a3, float b1, float b2, float coefp, float coefn)
{
	//image_display(id,h,w);
    float xp = 0.0f;  // previous input
    float yp = 0.0f;  // previous output
    float yb = 0.0f;  // previous output by 2
	for(int y=0;y<h;y++)
	{
		xp = id[y][0]; yb = coefp*xp; yp = yb;
		for (int x = 0; x < w; x++) 
		{
			float xc = id[y][x];
			float yc = a0*xc + a1*xp - b1*yp - b2*yb;
			od[y][x] = yc;
			xp = xc; yb = yp; yp = yc; 
		}
	}

	//image_display(od,h,w);
    // reverse pass
    // ensures response is symmetrical
    float xn = 0.f;
    float xa = 0.f;
    float yn = 0.f;
    float ya = 0.f;
	for(int y=0;y<h;y++)
	{
		xn = xa = id[y][w-1]; yn = coefn*xn; ya = yn;
		for (int x = w-1; x >= 0; x--) {
			float xc = id[y][x];
			float yc = a2*xn + a3*xa - b1*yn - b2*ya;
			xa = xn; xn = xc; ya = yn; yn = yc;
			od[y][x] = od[y][x]+ yc;
		}
	}
}
void gaussian_recursive_y(float **od,float **id, int w, int h, float a0, float a1, float a2, float a3, float b1, float b2, float coefp, float coefn)
{
    float xp = 0.0f;  // previous input
    float yp = 0.0f;  // previous output
    float yb = 0.0f;  // previous output by 2
	for (int x = 0; x < w; x++)
	{
		xp = id[0][x]; yb = coefp*xp; yp = yb;
		for(int y=0;y<h;y++)
		{
			float xc = id[y][x];
			float yc = a0*xc + a1*xp - b1*yp - b2*yb;
			od[y][x] = yc;
			xp = xc; yb = yp; yp = yc; 
		}
	}


    // reverse pass
    // ensures response is symmetrical
    float xn = 0.f;
    float xa = 0.f;
    float yn = 0.f;
    float ya = 0.f;
	for (int x = 0; x < w; x++)
	{
		xn = xa = id[h-1][x]; yn = coefn*xn; ya = yn;		
		for(int y=h-1;y>=0;y--)
		{
			float xc = id[y][x];
			float yc = a2*xn + a3*xa - b1*yn - b2*ya;
			xa = xn; xn = xc; ya = yn; yn = yc;
			od[y][x] = od[y][x]+ yc;
		}
	}
}
int gaussian_recursive(float **image,float **temp,float sigma,int order,int h,int w)
{
    const float
        nsigma = sigma < 0.1f ? 0.1f : sigma,
        alpha = 1.695f / nsigma,
        ema = exp(-alpha),
        ema2 = exp(-2*alpha),
        b1 = -2*ema,
        b2 = ema2;
    float a0 = 0, a1 = 0, a2 = 0, a3 = 0, coefp = 0, coefn = 0;
    switch (order) {
    case 0: {
        const float k = (1-ema)*(1-ema)/(1+2*alpha*ema-ema2);
        a0 = k;
        a1 = k*(alpha-1)*ema;
        a2 = k*(alpha+1)*ema;
        a3 = -k*ema2;
    } break;

    case 1: {
        const float k = (1-ema)*(1-ema)/ema;
        a0 = k*ema;
        a1 = a3 = 0;
        a2 = -a0;
    } break;

    case 2: {
        const float
            ea = exp(-alpha),
            k = -(ema2-1)/(2*alpha*ema),
            kn = (-2*(-1+3*ea-3*ea*ea+ea*ea*ea)/(3*ea+1+3*ea*ea+ea*ea*ea));
        a0 = kn;
        a1 = -kn*(1+k*alpha)*ema;
        a2 = kn*(1-k*alpha)*ema;
        a3 = -kn*ema2;
    } break;

    default:
        fprintf(stderr, "gaussianFilter: invalid order parameter!\n");
        return 0;
    }
    coefp = (a0+a1)/(1+b1+b2);
    coefn = (a2+a3)/(1+b1+b2);
	//timer.start();
	gaussian_recursive_x(temp,image,w,h,a0,a1,a2,a3,b1,b2,coefp,coefn);
	gaussian_recursive_y(image,temp,w,h,a0,a1,a2,a3,b1,b2,coefp,coefn);
	//timer.fps_display();
}


void qx_specular_free_image(unsigned char ***image_specular_free,unsigned char ***image_normalized,float **diffuse_chromaticity_max,int h,int w)
{
	int y,x;
	unsigned char *image_specular_free_x,*image_normalized_x; float *diffuse_chromaticity_max_x; 
	unsigned char r,g,b; double imax,isum; float rf,gf,bf,c,t0,t1,t2,t3,diffuse,specular;
	//*image_sum_x,*image_max_x,*chromaticity_max_x,
	image_specular_free_x=image_specular_free[0][0];
	image_normalized_x=image_normalized[0][0];
	diffuse_chromaticity_max_x=diffuse_chromaticity_max[0];
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			t1=3.f*(*diffuse_chromaticity_max_x++)-1.f; 
			t3=1.0f/3.0f;
			r=(*image_normalized_x++);
			g=(*image_normalized_x++);
			b=(*image_normalized_x++);
			if(t1>0)
			{
				isum=r+g+b;
				if(isum==0) c=0;
				else
				{
					imax=max(max(r,g),b);
					c=(float)(imax/isum);
				}
				t0=t1*c;
				if(fabs(t0)<QX_DEF_THRESHOLD_ZERO)
				{
					*image_specular_free_x++=r;
					*image_specular_free_x++=g;
					*image_specular_free_x++=b;
				}
				else
				{
					t2=(3.0f*c-1.f);
					diffuse=float(imax*t2/t0);
					specular=float(t3*(isum-diffuse));
					rf=r-specular;
					gf=g-specular;
					bf=b-specular;
					if(rf<0.f) rf=0.f; else if(rf>255.f) rf=255.f;
					if(gf<0.f) gf=0.f; else if(gf>255.f) gf=255.f;
					if(bf<0.f) bf=0.f; else if(bf>255.f) bf=255.f;
					*image_specular_free_x++=unsigned char(rf+0.5f);
					*image_specular_free_x++=unsigned char(gf+0.5f);
					*image_specular_free_x++=unsigned char(bf+0.5f);
				}
			}
			else
			{
				*image_specular_free_x++=r;
				*image_specular_free_x++=g;
				*image_specular_free_x++=b;
			}
		}
	}
}

void qx_stereo_flip_corr_vol(double***corr_vol_right,double***corr_vol,int h,int w,int nr_plane)
{	
	for(int y=0;y<h;y++)
	{
		for(int x=0;x<w-nr_plane;x++) for(int d=0;d<nr_plane;d++) corr_vol_right[y][x][d]=corr_vol[y][x+d][d];
		for(int x=w-nr_plane;x<w;x++) for(int d=0;d<nr_plane;d++)
		{
			if((x+d)<w) corr_vol_right[y][x][d]=corr_vol[y][x+d][d];
			else corr_vol_right[y][x][d]=corr_vol_right[y][x][d-1];
		}
	}
}
void depth_best_cost(unsigned char**depth,double***evidence,int h,int w,int nr_planes)
{
	for(int y=0;y<h;y++) for(int x=0;x<w;x++) {int d; vec_min_pos(d,evidence[y][x],nr_planes); depth[y][x]=d;}
}
void vec_min_pos(int &min_pos,double *in,int len)
{
	double min_val=in[0];
	min_pos=0;
	for (int i=1;i<len;i++) if(in[i]<min_val)
	{
		min_val=in[i];	
		min_pos= i;
	}
}
void qx_detect_occlusion_left_right(unsigned char**mask_left,unsigned char**depth_left,unsigned char**depth_right,int h,int w,int nr_plane)
{
	memset(mask_left[0],0,sizeof(char)*h*w);
	for(int y=0;y<h;y++) 
	{
		for(int x=0;x<w;x++) 
		{
			int d=depth_left[y][x];
			int xr=x-d;
			if(xr>=0)
			{
				if(d==0||abs(d-depth_right[y][xr])>=1)
				{
					//depth_left[y][x]=min(depth_left[y][x],depth_right[y][xr]);
					mask_left[y][x]=255;
				}
			}
			else mask_left[y][x]=255;
		}
	}
}

int file_open_ascii(char *file_path,int *out,int len)
{
	FILE *file_in; char str[65]; int i;
	//file_in=fopen(file_path,"r");
	fopen_s(&file_in,file_path,"r");
	if(file_in!=NULL)
	{
		fseek(file_in,0,SEEK_SET);	
		for(i=0;i<len;i++ )
		{ 
			//fscanf(file_in,"%s",str); 
			fscanf_s(file_in,"%s",str,65); 
			out[i]=atoi(str);
		}
		fclose(file_in);
	}
	else
	{
		printf("qx_basic_file: Can not open file: %s\n",file_path);
		getchar();
		exit(-1);
	}
	return(0);
}