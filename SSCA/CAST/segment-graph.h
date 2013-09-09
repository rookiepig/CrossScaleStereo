/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

//modified by X.Sun, 2012

#ifndef SEGMENT_GRAPH
#define SEGMENT_GRAPH

#define PENALTY_CROSS_SEG 5

#include <vector>
using namespace std;

#include <algorithm>
#include <cmath>
#include "disjoint-set.h"
#include "SegmentTree.h"

// threshold function
#define THRESHOLD(size, c) (c/size)
#define MIN_SIZE_SEG 50

/*
 * Segment a graph
 *
 * Returns a disjoint-set forest representing the segmentation.
 *
 * num_vertices: number of vertices in graph.
 * num_edges: number of edges in graph
 * edges: array of edges.
 * c: constant for threshold function.
 */
universe *segment_graph(int num_vertices, int num_edges, edge *edges, 
			float c, unsigned char *edges_mask) { 
  // sort edges by weight
  std::sort(edges, edges + num_edges);

  // make a disjoint-set forest
  universe *u = new universe(num_vertices);

  // init thresholds
  float *threshold = new float[num_vertices];
  for (int i = 0; i < num_vertices; i++)
    threshold[i] = THRESHOLD(1,c);
  
  // for each edge, in non-decreasing weight order...
  for (int i = 0; i < num_edges; i++) {
    edge *pedge = &edges[i];
    
    // components connected by this edge
    int a = u->find(pedge->a);
    int b = u->find(pedge->b);
    if (a != b) 
	{
		if (pedge->w <= threshold[a] && pedge->w <= threshold[b])
		{
			edges_mask[i]=255;
			u->join(a, b);
			a = u->find(a);	
			
			threshold[a]  = pedge->w + THRESHOLD(u->size(a), c);
		}
    }
  }

  //added by X. Sun: re-organizing the structures to be a single tree
  for (int i = 0; i < num_edges; i++)
  {
		int a = u->find(edges[i].a);
		int b = u->find(edges[i].b);
		if (a != b)
		{
			int size_min = MIN(u->size(a), u->size(b));
			u->join(a, b);

			//record
			edges_mask[i]=255;
			if(size_min > MIN_SIZE_SEG) 
				edges[i].w += PENALTY_CROSS_SEG;
		}
  }

  // free up
  delete []threshold;
  return u;
}

#endif