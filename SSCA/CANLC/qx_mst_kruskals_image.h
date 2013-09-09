/***********************************************************************************
\Author:	Qingxiong Yang
\Function:	Compute minimum spanning tree from an image using Kruskal's algorithm
			(a faster implementation can be obtained using Prim's algorithm).
\Reference:	Qingxiong Yang, A Non-Local Cost Aggregation Method for Stereo Matching,
			IEEE Conference on Computer Vision and Pattern Recognition (CVPR) 2012.
************************************************************************************/
#ifndef QX_MST_KRUSKALS_IMAGE_H
#define QX_MST_KRUSKALS_IMAGE_H
#define QX_DEF_MST_KI_MIN_DISTANT			1
#define QX_DEF_MST_KI_4NR_NEIGHBOR			4
#define QX_DEF_MST_KI_8NR_NEIGHBOR			8
#define QX_DEF_MST_KI_PARENT_DEFAULT		-1
#define QX_DEF_MST_KI_SIGMA_RANGE			0.1
#include "ctmf.h"
class qx_queue_mst
{
public:
	qx_queue_mst();
	~qx_queue_mst();
	void clean();
	int	init(int len);
	void reinit();
	void push(int x);
	int pull();//return x: the first item in the queue;
	int *queue;
	int	length;
	int	first;
	int	last;
};
class qx_mst_kruskals_image
{
public:
	qx_mst_kruskals_image();
	~qx_mst_kruskals_image();
	void clean();
	int init(int h,int w,int nr_channel,int nr_neighbor=QX_DEF_MST_KI_4NR_NEIGHBOR);
	int mst(unsigned char*image,bool print_edges=false);
public://output
	void update_table(double sigma_range);	
	int*get_rank(){return(m_rank);}
	int*get_parent(){return(m_parent);}
	int*get_nr_child(){return(m_nr_child);}
	int**get_children(){return(m_children);}
	unsigned char*get_weight(){return(m_weight);}
	int*get_node_id(){return(m_node_id_from_parent_to_child);}
private:
	int m_h,m_w,m_nr_channel,m_nr_neighbor;
	int m_nr_edge,m_nr_vertices,m_parent_default,m_tree_size;
	int**m_edge,*m_id_edge;//compute edges
	unsigned char*m_distance;
	unsigned char*m_image;
	
	int*m_parent;//obtain tree edges
	int*m_nr_child,m_max_nr_child,**m_children;
	unsigned char*m_weight;
	int**m_connected;
	unsigned char**m_connected_distance;
	int*m_nr_connected;

	qx_queue_mst m_queue;//build tree
	int*m_node_id_from_parent_to_child;
	int*m_rank;
private:
	void init_mst();
	int findset(int x);
	void kruskal();
	int build_tree();
	//void reorder_edges(vector< pair< int, QX_PAIR > >&min_spanning_tree);
	//void print(vector< pair< int, QX_PAIR > >&min_spanning_tree);
};
void test_qx_mst_kruskals_image();
#endif