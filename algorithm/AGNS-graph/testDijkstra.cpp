// Modification from:
// $\algorithms-nutshell-2ed\Code\Graph\SingleSourceShortestPath\testBellmanFord.cxx
// Chj modifies it to be compilable on Visual C++.

/**
 * @file testBellmanFord.cxx    test case for Bellman Ford
 * @brief 
 * 
 *   A test case
 *
 * @author George Heineman
 * @date 6/15/08
 */


#include <cstdio>
#include <cstdlib>
#include <string.h>

#ifdef UNIX
#include <getopt.h>
#include <sys/time.h>
#endif

#include "singleSourceShortest.h"

#include "Graph.h"
//#include "report.h"

#ifdef UNIX
/** Time before process starts.   */
static struct timeval before;

/** Time after process completes. */
static struct timeval after;
#endif // UNIX

void print_path_results(int vcount,
	const vector<int> &dist, const vector<int> &pred) 
{
	printf("      pred[]  dist[]\n");
	for (int i = 0; i < vcount; i++) {
		printf ("#%-2d.  %-6d   %-6d\n", i, pred[i], dist[i]);
	}
}

void test_AGNS2_p150()
{
	int n = 6; // vertex count is 5
	Graph g(n, true);

	g.addEdge(0, 1, 6); 
	g.addEdge(0, 3, 18);
	g.addEdge(0, 2, 8); 
	g.addEdge(1, 4, 11); 
	g.addEdge(2, 3, 9); 
	g.addEdge(4, 5, 3); 
	g.addEdge(5, 3, 4);
	g.addEdge(5, 2, 7);

	vector<int> dist(n);
	vector<int> pred(n);

	singleSourceShortest(g, 0, dist, pred);

	print_path_results(n, dist, pred);

/* Correct output:
		pred[]  dist[]
	#0 .  -1       0
	#1 .  0        6
	#2 .  0        8
	#3 .  2        17
	#4 .  1        17
	#5 .  4        20
*/
}

void test_chjCaseA()
{
	int n = 6; // vertex count is 5
	Graph g(n, true);
	g.addEdge(0, 2, 4); 
	g.addEdge(0, 1, 1);
	g.addEdge(2, 3, 2); 
	g.addEdge(1, 3, 3); 
	g.addEdge(3, 4, 2); 
	g.addEdge(3, 5, 2); 
	g.addEdge(1, 5, 9);

	vector<int> dist(n);
	vector<int> pred(n);
	singleSourceShortest(g, 0, dist, pred);
	print_path_results(n, dist, pred);
/*
        pred[]  dist[]
	#0 .  -1       0
	#1 .  0        1
	#2 .  0        4
	#3 .  1        4
	#4 .  3        6
	#5 .  3        6
*/
}

void test_chjCaseB()
{
	int n = 6; // vertex count is 5
	Graph g(n, true);
	g.addEdge(0, 2, 4); 
	g.addEdge(0, 1, 1);
	g.addEdge(2, 3, 2); 
	g.addEdge(1, 3, 6); // diff!
	g.addEdge(3, 4, 2); 
	g.addEdge(3, 5, 2); 
	g.addEdge(1, 5, 9);

	vector<int> dist(n);
	vector<int> pred(n);
	singleSourceShortest(g, 0, dist, pred);
	print_path_results(n, dist, pred);
/*
        pred[]  dist[]
	#0 .  -1       0
	#1 .  0        1
	#2 .  0        4
	#3 .  2        6
	#4 .  3        8
	#5 .  3        8
*/
}

#define RUNCASE(name) \
	printf("\nCase " #name ":\n");\
	name();

void run_stock_tests()
{
	RUNCASE(test_AGNS2_p150);
	
	RUNCASE(test_chjCaseA)
	RUNCASE(test_chjCaseB)
}

/** Launch program to load graph from a file an operate in verbose mode if needed. */
int main (int argc, char **argv) {
	bool verbose = true; // 0
	int idx;
	char *fileName = 0;

#ifdef UNIX
	opterr = 0;   // disable errors
	char c;
	while ((c = getopt(argc, argv, "f:v")) != -1) {
		idx++;
		switch (c) {

		case 'f':
			fileName = strdup (optarg);
			break;

		case 'v':
			verbose = 1;
			break;

		default:
			break;
		}
	}
	optind = 0;  // reset getopt for next time around.
#else // Windows:
	idx = 1;
	while(idx<argc)
	{
		if(strcmp(argv[idx], "-v")==0)
		{
			verbose = true;
		}
		else if(strcmp(argv[idx], "-f")==0)
		{
			fileName = argv[idx+1];
			idx++;
		}
		idx++;
	}
#endif

	if (fileName == 0 || *fileName=='\0') {
		printf ("You can load a graph datafile from command line.\n");
		printf ("  exename -f file\n");
		printf ("\n");
		printf ("Example:\n");
		printf ("  exename -f p150-dj.input.txt\n");
		printf ("\n");

		printf("Now run stock tests...\n");
		
		run_stock_tests();
		exit (0);
	}

	Graph graph(0);
	graph.load (fileName);

	int n = graph.numVertices();

	vector<int> dist(n);
	vector<int> pred(n);

	printf ("loaded graph with %d vertices (run Dijkstra)\n", n);
	try {
//		gettimeofday(&before, (struct timezone *) NULL);    // begin time
		singleSourceShortest(graph, 0, dist, pred);
//		gettimeofday(&after, (struct timezone *) NULL);     // end time

//		long usecs = diffTimer (&before, &after);           // show results
//		printf("%s\n", timingString(usecs));

		if (verbose) {
			print_path_results(n, dist, pred);
		}
	} 
	catch (char const *s) {
		printf ("%s\n", s);
	}
}
