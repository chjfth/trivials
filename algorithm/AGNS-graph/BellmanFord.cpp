// $\algorithms-nutshell-2ed\Code\Graph\SingleSourceShortestPath\bellmanFord.cxx
// AGNS2 book p156

/**
 * @file bellmanFord.cxx    BellmanFord implementation of Single Source Shortest Path
 * @brief 
 *   Contains BellmanFord implementation for solving Single Source Shortest Path problems.
 *
 * @author George Heineman
 * @date 6/15/08
 */

#include <cstdio>

#include "Graph.h"

/** Useful debugging method. */
void output (int n, 
	     vector<int> &dist, vector<int> &pred) { /* out */
  printf ("dist: ");
  for (int i = 0; i < n; i++) {
    printf ("%d ", dist[i]);
  }
  printf ("\npred: ");
  for (int i = 0; i < n; i++) {
    printf ("%d ", pred[i]);
  }
  printf ("\n");
}

/**
 * Given directed, weighted graph, compute shortest distance to all vertices
 * in graph (dist) and record predecessor links for all vertices (pred) to
 * be able to recreate these paths. Graph weights can be negative so long
 * as there are no negative cycles.
 * \param graph   the graph to be processed.
 * \param s       the source vertex from which to compute all paths.
 * \param dist    array to contain shortest distances to all other vertices.
 * \param pred    array to contain previous vertices to be able to recompute paths.
 */
void singleSourceShortest_BellmanFord(Graph const &graph, int s, /* in */
	vector<int> &dist, vector<int> &pred, /* out */
	bool is_reverse_feed
	)
{
	// initialize dist[] and pred[] arrays.
	const int n = graph.numVertices();
	pred.assign(n, -1);
	dist.assign(n, numeric_limits<int>::max()/2); 
		// chj: should assign max/2, otherwise, newLen below may wrap to a negative value.
	dist[s] = 0;

	int ucount = 0; // update count

	// After n-1 times we can be guaranteed distances from s to all 
	// vertices are properly computed to be shortest. So on the nth 
	// pass, a change to any value guarantees there is negative cycle.
	// Leave early if no changes are made.
	int i;
	for (i = 1; i <= n; i++) 
	{
		bool failOnUpdate = (i == n);
		bool leaveEarly = true;

		// Process each vertex, u, and its respective edges to see if 
		// some edge (u,v) realizes a shorter distance from s->v by going
		// through s->u->v. Use longs to prevent overflow.
		for (int uadv = 0; uadv < n; uadv++)
		{
			int u = !is_reverse_feed ? uadv : n-1-uadv;

			for (VertexList::const_iterator ci = graph.begin(u);
				ci != graph.end(u); 
				++ci) 
			{
				int v = ci->first;
				long newLen = dist[u];
				newLen += ci->second;
				if (newLen < dist[v]) 
				{
					if (failOnUpdate) { 
						throw "Graph has negative cycle"; 
					}
					dist[v] = newLen; 
					pred[v] = u;
					leaveEarly = false;

					ucount++;
				}
			}
		}
		
		if (leaveEarly) { 
			// We can observe ucount and i here.
			// With different is_reverse_feed, we see different value of ucount and i.
			break; 
		}
	}
}

void singleSourceShortest(Graph const &graph, int s,
	vector<int> &dist, vector<int> &pred)
{
	return singleSourceShortest_BellmanFord(graph, s, dist, pred, false);
}
