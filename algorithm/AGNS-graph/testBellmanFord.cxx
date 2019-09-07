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
	printf("      dist[]  pred[]\n");
	for (int i = 0; i < vcount; i++) {
		printf ("#%-2d.  %-6d   %-6d\n", i, dist[i], pred[i]);
	}
}

void test_AGNS2_p155_case1()
{
	int n = 5; // vertex count is 5
	Graph g(n, true);

	g.addEdge(0, 4, 2); 
	g.addEdge(4, 3, 4); 
	g.addEdge(4, 1, 5); 
	g.addEdge(3, 2, 6); 
	g.addEdge(2, 1, -3); 
	g.addEdge(1, 3, -2);

	vector<int> dist(n);
	vector<int> pred(n);

	singleSourceShortest(g, 0, dist, pred);

	print_path_results(n, dist, pred);
}


void run_stock_tests()
{
	test_AGNS2_p155_case1();
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
		printf ("  exename [-v] -f file\n");
		printf ("\n");
		printf ("Example:\n");
		printf ("  exename -f testBellmanFord.input.txt\n");
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

	printf ("loaded graph with %d vertices\n", n);
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

