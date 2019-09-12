// $\algorithms-nutshell-2ed\Code\Graph\MinimumSpanningTree\testCormen.cxx
/**
 * @file testCormen.cxx   Small test case for MST
 * @brief 
 *   A test case.
 *
 * @author George Heineman
 * @date 6/15/08
 */

#include <cassert>
#include <iostream>
#include "mst.h"

/** Cormen (2nd edition), p. 571 */
void main_testcase () 
{
  int n = 9;
  Graph g(n, false);

  g.addEdge (0,1,4);
  g.addEdge (0,7,8);
  g.addEdge (1,2,8);
  g.addEdge (1,7,11);
  g.addEdge (2,8,2);
  g.addEdge (2,3,7);
  g.addEdge (2,5,4);
  g.addEdge (3,4,9);
  g.addEdge (4,5,10);
  g.addEdge (5,6,2);
  g.addEdge (6,7,1);
  g.addEdge (6,8,6);
  g.addEdge (7,8,7);

  vector<int> pred(n);

  mst_prim (g, pred);

  assert (pred[0] == -1);
  assert (pred[1] == 0);
  assert (pred[2] == 1);
  assert (pred[3] == 2);
  assert (pred[4] == 3);
  assert (pred[5] == 2);
  assert (pred[6] == 5);
  assert (pred[7] == 6);
  assert (pred[8] == 2);

  cout << "Tests passed\n";
}

void print_result(const Graph &graph, const int pred[], int start_node)
{
	printf("Prim's find MST result: (start_node=#%d)\n", start_node);

	int n = graph.numVertices();
	// pred[] contains exactly n elements.

	int wtotal = 0;
	int iedge = 0;
	for(int i=0; i<n; i++)
	{
		if(pred[i]==-1) {
			assert(i==start_node);
			continue; // the algorithm starting point
		}

		iedge++;

		int w = graph.edgeWeight(pred[i], i);
		wtotal += w;
		printf("$%-2d: (%-2d,%-2d) weight: %d\n", 
			iedge, pred[i], i, w);
	}
	printf("Total weight of MST: %d\n", wtotal);
}

void test_prims_p165(int start_node)
{
	int n = 5;
	Graph graph(n, false); // undirected graph
	graph.addEdge(0, 1, 2);
	graph.addEdge(0, 4, 4);
	graph.addEdge(0, 3, 8);
	graph.addEdge(1, 2, 3);
	graph.addEdge(4, 2, 1);
	graph.addEdge(4, 3, 7);
	graph.addEdge(3, 2, 5);

	vector<int> pred(n);
	mst_prim(graph, pred, start_node);

	print_result(graph, &pred[0], start_node);
}

void run_stock_tests()
{
	test_prims_p165(0);
	test_prims_p165(4);

	main_testcase();
}

int main (int argc, char **argv) 
{
	int idx;
	char *fileName = 0;

	idx = 1;
	while(idx<argc)
	{
		if(strcmp(argv[idx], "-f")==0)
		{
			fileName = argv[idx+1];
			idx++;
		}
		idx++;
	}

	if (fileName == 0 || *fileName=='\0') {
		printf ("You can load a graph datafile from command line.\n");
		printf ("  exename -f file\n");
		printf ("\n");
		printf ("Example:\n");
		printf ("  exename -f p165-mst.input.txt\n");
		printf ("\n");

		printf("Now run stock tests...\n");

		run_stock_tests();
		exit (0);
	}

	Graph graph(0);
	graph.load (fileName);

	int n = graph.numVertices();

	vector<int> pred(n);

	printf ("loaded graph with %d vertices (run Prims)\n", n);

	mst_prim(graph, pred, 0);

	print_result(graph, &pred[0], 0);
}