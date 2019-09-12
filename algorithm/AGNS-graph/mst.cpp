// $\algorithms-nutshell-2ed\Code\Graph\MinimumSpanningTree\mst.cxx

/**
 * @file mst.cxx    Prim's Algorithm for Minimum Spanning Tree problem
 * @brief 
 *
 *   Defines the implementation using Prim's Algorithm to minimum spanning
 *   tree problem
 *
 * @author George Heineman
 * @date 6/15/08
 */

#include <iostream>

#include "BinaryHeap2.h"
#include "Graph.h"

/** Useful method for Debugging. */
void debug (int n, vector<int> key, vector<int> pred) {
  cout << "n  key  pred\n";
  for (unsigned int i = 0; i < (unsigned int) n; i++) {
    cout << i << ". " << key[i] << "  " << pred[i] << endl;
  }
  cout << "--------" << endl;
}

/**
 * Given undirected graph, compute MST starting from a randomly 
 * selected vertex. Encoding of MST is done using 'pred' entries.
 * \param graph    the undirected graph
 * \param pred     pred[] array to contain previous information for MST.
 * \param start_node    Start algorithm from which node,
 *                      No matter which node to start, the result should be same.
 */
bool mst_prim (Graph const &graph, vector<int> &pred, int start_node) 
{
	// initialize pred[] and key[] arrays. Start with arbitrary 
	// vertex s=0. Priority Queue PQ contains all v in G.
	const int n = graph.numVertices();
	pred.assign(n, -1);
	vector<int> key(n, numeric_limits<int>::max());

	if(start_node<0 || start_node>=n)
		return false;

	key[start_node] = 0;

	BinaryHeap pq(n);
	vector<bool>  inQueue(n, true);
	for (int v = 0; v < n; v++) 
		pq.insert(v, key[v]);

	while (!pq.isEmpty()) 
	{
		int u = pq.smallest(); // the node u is removed from pq
		inQueue[u] = false;

		// Process all neighbors of u to find if any edge beats best distance
		for (VertexList::const_iterator ci = graph.begin(u);
			ci != graph.end(u); 
			++ci) 
		{
			int v = ci->first;
			if (!inQueue[v]) 
				continue;

			int w = ci->second; // w: weight
			if (w < key[v]) 
			{
				pred[v] = u;
				key[v] = w;
				pq.decreaseKey (v, w);
			}
		}
	}

	return true;
}
