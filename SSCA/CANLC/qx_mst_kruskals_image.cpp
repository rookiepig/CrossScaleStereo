
#include "qx_basic.h"
#include "qx_mst_kruskals_image.h"

inline int qx_mst_yx_2_image_index(int y,int x,int h,int w){return(y*w+x);}
inline int qx_mst_compute_nr_edge_4neighbor(int h,int w){if(h<=2&&w<=2) return(0); else return((h-1)*w+(w-1)*h);}
inline int qx_mst_compute_nr_edge_8neighbor(int h,int w){if(h<=2&&w<=2) return(0); else return((h-1)*w+(w-1)*h+(h-1)*(w-1)*2);}
inline void qx_mst_compute_edges_per_pixel(int**edges,unsigned char*distance,unsigned char*image,int nr_channel,int &nr_edge,int y0,int x0,int yt,int xt,int h,int w)
{
	int id0=edges[nr_edge][0]=qx_mst_yx_2_image_index(y0,x0,h,w);
	int idt=edges[nr_edge][1]=qx_mst_yx_2_image_index(yt,xt,h,w);
	if(nr_channel==1) 
		distance[nr_edge]=abs(image[idt]-image[id0]);
		//edges[nr_edge++][2]=min(10,abs(image[edges[nr_edge][1]]-image[edges[nr_edge][0]])); 
	else if(nr_channel==3) 
	{
		id0*=nr_channel;
		idt*=nr_channel;
		//distance[nr_edge]=euro_dist_rgb_max(&(image[idt]),&(image[id0]));

		
		unsigned char r=abs(image[idt++]-image[id0++]);
		unsigned char g=abs(image[idt++]-image[id0++]);
		unsigned char b=abs(image[idt++]-image[id0++]);
		distance[nr_edge]=qx_max_u3(r,g,b);
		//distance[nr_edge]=int((r+g+b)*0.33+0.5);
		//double e=(r+g+b)*0.5;//euro_dist_rgb_mean(&(image[edges[nr_edge][1]*3]),&(image[edges[nr_edge][0]*3]));
		//edges[nr_edge++][2]=min(10,euro_dist_rgb_max(&(image[edges[nr_edge][1]*3]),&(image[edges[nr_edge][0]*3])));
		//if(e<QX_DEF_MST_KI_MIN_DISTANT) distance[nr_edge]=0;
		//else distance[nr_edge]=min(255,int(e+0.5));

	}
	else
	{
		id0*=nr_channel;
		idt*=nr_channel;
		int cost_max=0;
		for(int i=0;i<nr_channel;i++) 
		{
			cost_max=max(cost_max,abs(image[idt+i]-image[id0+i]));
		}
		distance[nr_edge]=cost_max;
	}
	nr_edge++;
}
inline int qx_mst_compute_edges_4neighbor(int**edges,unsigned char*distance,unsigned char*image,int nr_channel,int h,int w)
{
	int y0,yt,x0,xt;
	int nr_edge=0;
	for(y0=0;y0<h;y0++)
	{
		yt=y0;
		for(int x0=0;x0<w-1;x0++)
		{
			xt=x0+1;
			qx_mst_compute_edges_per_pixel(edges,distance,image,nr_channel,nr_edge,y0,x0,yt,xt,h,w);
		}
	}
	for(int x0=0;x0<w;x0++)
	{
		xt=x0;
		for(y0=0;y0<h-1;y0++)
		{
			yt=y0+1;
			qx_mst_compute_edges_per_pixel(edges,distance,image,nr_channel,nr_edge,y0,x0,yt,xt,h,w);
		}
	}
	return(nr_edge);
}

inline int qx_mst_compute_edges_8neighbor(int**edges,unsigned char*distance,unsigned char*image,int nr_channel,int h,int w)
{
	int y0,yt,x0,xt;
	int nr_edge=qx_mst_compute_edges_4neighbor(edges,distance,image,nr_channel,h,w);
	for(y0=0;y0<h-1;y0++)
	{
		yt=y0+1;
		for(int x0=0;x0<w-1;x0++)
		{
			xt=x0+1;
			qx_mst_compute_edges_per_pixel(edges,distance,image,nr_channel,nr_edge,y0,x0,yt,xt,h,w);
			qx_mst_compute_edges_per_pixel(edges,distance,image,nr_channel,nr_edge,y0,xt,yt,x0,h,w);
		}
	}
	return(nr_edge);
}


qx_mst_kruskals_image::qx_mst_kruskals_image()
{
	m_image=NULL;
	m_edge=NULL;
	m_distance=NULL;
	m_id_edge=NULL;

	m_parent=NULL;
	m_nr_child=NULL;
	m_children=NULL;
	m_weight=NULL;

	
	m_connected=NULL;
	m_connected_distance=NULL;
	m_nr_connected=NULL;

	m_node_id_from_parent_to_child=NULL;
	m_rank=NULL;

	m_parent_default=QX_DEF_MST_KI_PARENT_DEFAULT;
}
qx_mst_kruskals_image::~qx_mst_kruskals_image()
{
	clean();
}
void qx_mst_kruskals_image::clean()
{
	qx_freeu_1(m_image); m_image=NULL;
	qx_freei(m_edge); m_edge=NULL;
	qx_freeu_1(m_distance); m_distance=NULL;
	qx_freei_1(m_id_edge); m_id_edge=NULL;
	
	qx_freei_1(m_parent); m_parent=NULL;
	qx_freei_1(m_nr_child); m_nr_child=NULL;
	qx_freeu_1(m_weight); m_weight=NULL;
	qx_freei(m_children); m_children=NULL;
	
	qx_freei(m_connected); m_connected=NULL;
	qx_freeu(m_connected_distance); m_connected_distance=NULL;
	qx_freei_1(m_nr_connected); m_nr_connected=NULL;

	qx_freei_1(m_node_id_from_parent_to_child); m_node_id_from_parent_to_child=NULL;
	qx_freei_1(m_rank); m_rank=NULL;
}
int qx_mst_kruskals_image::init(int h,int w,int nr_channel,int nr_neighbor)
{
	clean();
	m_h=h; m_w=w; m_nr_channel=nr_channel; m_nr_neighbor=nr_neighbor;
	m_max_nr_child=m_nr_neighbor-1;
	m_nr_vertices=m_h*m_w;
	if(m_nr_neighbor==QX_DEF_MST_KI_4NR_NEIGHBOR) m_nr_edge=qx_mst_compute_nr_edge_4neighbor(m_h,m_w);
	else m_nr_edge=qx_mst_compute_nr_edge_8neighbor(m_h,m_w);
	m_image=new unsigned char [m_h*m_w*m_nr_channel];
	m_edge=qx_alloci(m_nr_edge,2);
	m_distance=new unsigned char[m_nr_edge];
	m_id_edge=new int[m_nr_edge];
	
	m_parent=new int [m_nr_vertices];
	m_nr_child=new int [m_nr_vertices];
	m_weight=new unsigned char [m_nr_vertices];
	m_children=qx_alloci(m_nr_vertices,m_max_nr_child);
	
	m_connected=qx_alloci(m_nr_vertices,m_nr_neighbor);
	m_connected_distance=qx_allocu(m_nr_vertices,m_nr_neighbor);
	m_nr_connected=new int[m_nr_vertices];
	
	m_node_id_from_parent_to_child=new int [m_nr_vertices];//build tree
	m_rank=new int [m_nr_vertices];
	m_queue.init(m_nr_vertices);
	return(0);
}
void qx_mst_kruskals_image::init_mst()
{
	int*parent=m_parent; 
	for(int i=0;i<m_nr_vertices;i++) *parent++=i;
	memset(m_nr_connected,0,sizeof(int)*m_nr_vertices);
}
int qx_mst_kruskals_image::mst(unsigned char*image,bool print_edges)
{
	qx_timer timer;
	timer.start();
	init_mst();
	//timer.fps_display();
	timer.start();
	ctmf(image,m_image,m_w,m_h,m_w*m_nr_channel,m_w*m_nr_channel,1,m_nr_channel,m_h*m_w*m_nr_channel);
	if(m_nr_neighbor==QX_DEF_MST_KI_4NR_NEIGHBOR) qx_mst_compute_edges_4neighbor(m_edge,m_distance,m_image,m_nr_channel,m_h,m_w);//find edges
	else qx_mst_compute_edges_8neighbor(m_edge,m_distance,m_image,m_nr_channel,m_h,m_w);
	//timer.fps_display();
	timer.start();
	qx_sort_increase_using_histogram(m_id_edge,m_distance,m_nr_edge);//sorting edges in a nondesc
	//timer.fps_display("qx_sort_increase_using_histogram");
	timer.start();
	kruskal();
	//timer.fps_display("kruskal");
	build_tree();
	return(0);
}

int qx_mst_kruskals_image::findset(int x)
{
	//if(x==m_nr_vertices) printf("[%d - %d]",x,m_nr_vertices);
	int parent=m_parent[x];
    if(x!=parent)
	{
        m_parent[x]=findset(parent);
	}
    return m_parent[x];
}
void qx_mst_kruskals_image::kruskal()
{
	m_tree_size=0;
    for(int j=0;j<m_nr_edge;j++)
    {
		int i=m_id_edge[j];
		int*edge=m_edge[i];
		int u=edge[0];
		int v=edge[1];
        int pu=findset(u);
        int pv=findset(v);
        if(pu!=pv)
        {
			int nr_connected=m_nr_connected[u];
			m_connected[u][nr_connected]=v;
			m_connected_distance[u][nr_connected]=m_distance[i];
			m_nr_connected[u]++;

			nr_connected=m_nr_connected[v];
			m_connected[v][nr_connected]=u;
			m_connected_distance[v][nr_connected]=m_distance[i];
			m_nr_connected[v]++;

			m_tree_size++;
			//printf("( %d, %d ): %d\n", u,v,m_distance[i]);
            m_parent[pu]=m_parent[pv]; // link
        }
    }
	//printf("m_total_weight: %d\n",total_weight);
	//printf("size: [%d - %d]\n",m_tree_size,m_h*m_w);
}
int qx_mst_kruskals_image::build_tree()
{
	qx_timer timer;
	timer.start();
	int tree_parent=0;
	int parent=tree_parent;
	memset(m_parent,m_parent_default,sizeof(int)*m_nr_vertices);
	memset(m_nr_child,0,sizeof(int)*m_nr_vertices);
	memset(m_rank,0,sizeof(int)*m_nr_vertices);
	
	m_parent[parent]=parent;
	m_weight[parent]=0;
	m_node_id_from_parent_to_child[0]=parent;
	int len=1;
	m_queue.reinit();
	m_queue.push(parent);
	//m_max_rank=0; 
	while(m_queue.length>0)
	{
		parent=m_queue.pull();
		int nr_connected=m_nr_connected[parent];
		for(int i=0;i<nr_connected;i++)
		{
			int potential_child=m_connected[parent][i];
			if(m_parent[potential_child]==m_parent_default)//&&m_parent[potential_child]!=tree_parent)
			{
				m_queue.push(potential_child);
				m_parent[potential_child]=parent;
				//printf("[%d <- %d]\n",parent,potential_child);
				m_rank[potential_child]=m_rank[parent]+1;
				unsigned char weight=m_connected_distance[parent][i];
				m_weight[potential_child]=weight;
				//m_nodes[potential_child].value_to_be_filtered=m_image[potential_child];
				m_children[parent][(m_nr_child[parent]++)]=potential_child;

				m_node_id_from_parent_to_child[len]=potential_child;
				len++;
				if(len>m_nr_vertices)
				{
					printf("len>m_nr_vertices!!");
					getchar();
					exit(0);
				}
			}
		}
	}
	//timer.fps_display("build_tree");
	return(0);
}
void test_qx_mst_kruskals_image()
{
	qx_mst_kruskals_image m_mst;
	int h=3,w=3;
	//unsigned char***a=loadimage_ppm_u("a.ppm",h,w);
	unsigned char aa[3][3]={0,1,2,3,4,5,6,7,8};
	unsigned char**a=qx_allocu(h,w);
	for(int y=0;y<h;y++) for(int x=0;x<w;x++) a[y][x]=aa[y][x];
	m_mst.init(h,w,1);
	m_mst.mst(a[0],true);
	qx_freeu(a);
}


qx_queue_mst::qx_queue_mst()
{
	queue=NULL;
};
qx_queue_mst::~qx_queue_mst()
{
	clean();
};
void qx_queue_mst::clean() 
{
	if(queue!=NULL) delete [] queue; queue=NULL;
}
int qx_queue_mst::init(int len)
{
	if (len<=0) 
	{
		cout<<"The length is: "<<len<<"!!";
		exit (0);
	}
	queue=new int [len*2];
	first=0; 
	last=-1;
	length=0;
	return(0);
}
void qx_queue_mst::reinit() 
{
	first=0; 
	last=-1;
	length=0;
}
void qx_queue_mst::push(int x) 
{
	length++;
	queue[++last]=x;
}
int qx_queue_mst::pull()
{
	length--;
	return(queue[first++]);
}
