/**
 * @file Graph/AllPairsShortestPath/test1.cxx   Test Case for Floyd-Warshall
 * @brief 
 *   A test case
 *
 * @author George Heineman
 * @date 6/15/08
 */

#include <string.h>
#include <assert.h>
#include <iostream>
#include <cassert>

#include "allPairsShortest.h"

/** Helper method for asserting a path from expected values. */
void assertPath(int s, int t, 
		vector< vector<int> > const &pred, int expect[]) {

  list<int> path(0);
  constructShortestPath(s, t, pred, path);

  int i = 0;
  for (list<int>::const_iterator ci = path.begin();
       ci != path.end(); ++ci) {
    assert (expect[i++] == *ci);
  }
}


/** test case from Cormen, 2nd edition, p. 626 */
int main_testcase () {
  int n = 5;
  Graph g (n, true);

  // cormen counts from 1. we count from 0.
  g.addEdge (0, 1, 3);
  g.addEdge (0, 2, 8);
  g.addEdge (0, 4, -4);
  g.addEdge (1, 4, 7);
  g.addEdge (1, 3, 1);
  g.addEdge (2, 1, 4);
  g.addEdge (3, 0, 2);
  g.addEdge (3, 2, -5);
  g.addEdge (4, 3, 6);

  vector< vector<int> > dist(n, vector<int>(n));
  vector< vector<int> > pred(n, vector<int>(n));

  allPairsShortest(g, dist, pred);

  int results[5][5] = {
    {0,1,-3,2,-4},
    {3,0,-4,1,-1},
    {7,4,0,5,3},
    {2,-1,-5,0,-2},
    {8,5,1,6,0}};

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      //      printf ("%d %d %d %d\n", i, j, dist[i][j], results[i][j]);
      assert (dist[i][j] == results[i][j]);
    }
  }

  // validate some shortest paths.
  int res[] = {4,3,2,1};
  assertPath (4, 1, pred, res);

  int res2[] = {4,3,0};
  assertPath (4, 0, pred, res2);

  cout << "Test passed.\n";
  return 0;
}

void print_matrix_results(int n, 
	vector< vector<int> > &matrix,
	const char *desc)
{
	int fr, to;

	printf("[%.4s] To:", desc);
	for(to=0; to<n; to++)
		printf(".#%02d.", to);
	printf("\n");
	for(fr=0; fr<n; fr++)
	{
		const vector<int> &arto = matrix[fr];
		printf("From #%02d :", fr);
		for(to=0; to<n; to++)
		{
			printf(" %-3d ", arto[to]);
		}
		printf("\n");
	}
}

void test_p160()
{
	int n = 5;
	Graph graph (n, true);

	graph.addEdge(0, 1, 2);
	graph.addEdge(0, 4, 4);
	graph.addEdge(3, 0, 8);
	graph.addEdge(1, 2, 3);
	graph.addEdge(2, 4, 1);
	graph.addEdge(2, 3, 5);
	graph.addEdge(4, 3, 7);

	vector< vector<int> > dist(n, vector<int>(n));
	vector< vector<int> > pred(n, vector<int>(n));
	allPairsShortest(graph, dist, pred);

	printf("\n");
	print_matrix_results(n, dist, "dist");
	printf("\n");
	print_matrix_results(n, pred, "pred");
/*
	[dist] To:.#00..#01..#02..#03..#04.
	From #00 : 0    2    5    10   4
	From #01 : 16   0    3    8    4
	From #02 : 13   15   0    5    1
	From #03 : 8    10   13   0    12
	From #04 : 15   17   20   7    0

	[pred] To:.#00..#01..#02..#03..#04.
	From #00 : -1   0    1    2    0
	From #01 : 3    -1   1    2    2
	From #02 : 3    0    -1   2    2
	From #03 : 3    0    1    -1   0
	From #04 : 3    0    1    4    -1
*/
}

void run_stock_tests() 
{
	test_p160();

	main_testcase();
}

int main(int argc, char *argv[]) 
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
		printf ("  exename -f p160-allshortest.input.txt\n");
		printf ("\n");

		printf("Now run stock tests...\n");

		run_stock_tests();
		exit (0);
	}

	Graph graph(0);
	graph.load (fileName);

	int n = graph.numVertices();

	vector< vector<int> > dist(n, vector<int>(n));
	vector< vector<int> > pred(n, vector<int>(n));

	printf ("loaded graph with %d vertices (run Floyd-Warshall)\n", n);

	allPairsShortest(graph, dist, pred);

	printf("\n");
	print_matrix_results(n, dist, "dist");
	printf("\n");
	print_matrix_results(n, pred, "pred");

	return 0;
}
