#include "qx_basic.h"
#include "qx_tree_filter.h"

qx_tree_filter::qx_tree_filter()
{
}
qx_tree_filter::~qx_tree_filter()
{
	clean();
}
void qx_tree_filter::clean()
{
}
int qx_tree_filter::init(int h,int w,int nr_channel,double sigma_range,int nr_neighbor)
{
	m_h=h; m_w=w; m_nr_pixel=m_h*m_w;
	m_mst.init(m_h,m_w,nr_channel,nr_neighbor);
	update_table(sigma_range);
	return(0);
}
void qx_tree_filter::update_table(double sigma_range)
{
	sigma_range=max(0.01,sigma_range);
	for(int i=0;i<=QX_DEF_CHAR_MAX;i++) m_table[i]=exp(-double(i)/(QX_DEF_CHAR_MAX*sigma_range));//weight table
}
int qx_tree_filter::build_tree(unsigned char*texture)
{
	m_mst.mst(texture);
	m_mst_parent=m_mst.get_parent();
	m_mst_nr_child=m_mst.get_nr_child();
	m_mst_children=m_mst.get_children();
	m_mst_rank=m_mst.get_rank();
	m_mst_weight=m_mst.get_weight();
	m_node_id=m_mst.get_node_id();

	return(0);
}
template<typename T>void qx_tree_filter::combine_tree(T*image_filtered)
{
	double*value=m_mst_value_sum_aggregated_from_parent_to_child;
	for(int i=0;i<m_nr_pixel;i++)
	{
		*image_filtered++=*value++;//every slices will have the same weight, thus don't need normalization.
	}
}
template<typename T>void qx_tree_filter::init_tree_value(T*image,bool compute_weight)
{
	memset(m_mst_value_sum_aggregated_from_child_to_parent,0,sizeof(double)*m_nr_pixel);
	memset(m_mst_value_sum_aggregated_from_parent_to_child,0,sizeof(double)*m_nr_pixel);
	if(compute_weight)
	{
		memset(m_mst_weight_sum_aggregated_from_child_to_parent,0,sizeof(double)*m_nr_pixel);
		memset(m_mst_weight_sum_aggregated_from_parent_to_child,0,sizeof(double)*m_nr_pixel);/*nodes*/
	}
	double*value_to_be_filtered=m_mst_value_to_be_filtered;
    for(int i=0;i<m_nr_pixel;i++) 
	{
		*value_to_be_filtered++=double(*image++);
	}
}
int qx_tree_filter::filter(double*cost,double*cost_backup,int nr_plane)
{
	memcpy(cost_backup,cost,sizeof(double)*m_h*m_w*nr_plane);
	int*node_id=m_node_id;
	int*node_idt=&(node_id[m_nr_pixel-1]);
	for(int i=0;i<m_nr_pixel;i++)
	{
		int id=*node_idt--;
		int id_=id*nr_plane;
		int nr_child=m_mst_nr_child[id];
		if(nr_child>0)
		{
			double*value_sum=&(cost_backup[id_]);
			for(int j=0;j<nr_child;j++)
			{
				int id_child=m_mst_children[id][j];
				int id_child_=id_child*nr_plane;
				double weight=m_table[m_mst_weight[id_child]];
				//value_sum+=m_mst_value_sum_aggregated_from_child_to_parent[id_child]*weight;
				double*value_child=&(cost_backup[id_child_]);
				for(int k=0;k<nr_plane;k++)
				{
					value_sum[k]+=(*value_child++)*weight;
				}
			}
		}
		//else
		//{
		//	memcpy(&(cost_backup[id_]),&(cost[id_]),sizeof(double)*nr_plane);
		//}
		//printf("[id-value-weight]: [%d - %3.3f - %3.3f]\n",id,m_mst_[id].value_sum_aggregated_from_child_to_parent,m_mst_[id].weight_sum_aggregated_from_child_to_parent);
	}
	int*node_id0=node_id;
	int tree_parent=*node_id0++;
	int tree_parent_=tree_parent*nr_plane;
	memcpy(&(cost[tree_parent_]),&(cost_backup[tree_parent_]),sizeof(double)*nr_plane);
	for(int i=1;i<m_nr_pixel;i++)//K_00=f(0,00)[K_0-f(0,00)J_00]+J_00, K_00: new value, J_00: old value, K_0: new value of K_00's parent
	{
		int id=*node_id0++;
		int id_=id*nr_plane;
		int parent=m_mst_parent[id];
		int parent_=parent*nr_plane;

		double*value_parent=&(cost[parent_]);//K_0
		double*value_current=&(cost_backup[id_]);//J_00
		double*value_final=&(cost[id_]);
		double weight=m_table[m_mst_weight[id]];//f(0,00)

		for(int k=0;k<nr_plane;k++) 
		{
			double vc=*value_current++;
			*value_final++=weight*((*value_parent++)-weight*vc)+vc;
		}
		//printf("Final: [id-value-weight]: [%d - %3.3f - %3.3f]\n",id,m_mst_[id].value_sum_aggregated_from_parent_to_child,m_mst_[id].weight_sum_aggregated_from_parent_to_child);
	}
	return(0);
}
