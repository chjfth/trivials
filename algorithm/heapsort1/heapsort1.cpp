// Origin: $\algorithms-nutshell-2ed\Code\Sorting\PointerBased\straight_HeapSort.c

#include <stdio.h>
#include <assert.h>
#include <string.h>

//#define MAXELE 100
#define COUNT(ar) (sizeof(ar)/sizeof(ar[0]))
int g_cases = 0;

/**
 * @file straight_HeapSort.c      Generic Heap Sort implementation.
 * @brief
 *  A recursive Heap Sort implementation.
 * 
 * @author George Heineman
 * @date 6/15/08
 */

//#include "report.h"

typedef int DATATYPE; // orig: typedef void *DATATYPE;

void swap(DATATYPE &a, DATATYPE &b)
{
	DATATYPE tmp = a;
	a = b;
	b = tmp;
}

/** Heapify the subarray ar[idx,max). */
static void heapify (DATATYPE *ar, 
	int(*cmp)(const DATATYPE,const DATATYPE), 
	int idx, int max) 
{
	// [2019-09-02] Chj memo:
	// cmp() means "is param1 larger then param2".
	// The largest-value node will be at tree root.
	//
	// Important input premise: All nodes, except current root node(indicated by idx), 
	// have been in heapified state.
	//
	// The root node now may be the "out-of-order" node, so this function will 
	// move(swap) that out-of-order root to its proper location, so that 
	// on function return, the tree at idx has become a heapified tree.

	int left = 2*idx + 1;
	int right = 2*idx + 2;
	int largest;

	/* Find largest element of A[idx], A[left], and A[right]. */
	if (left < max && cmp (ar[left], ar[idx]) > 0) {
		largest = left;
	} else {
		largest = idx;
	}

	if (right < max && cmp(ar[right], ar[largest]) > 0) {
		largest = right;
	}

	/* If largest is not already the parent then swap and propagate. */
	if (largest != idx) {
		swap(ar[idx], ar[largest]);
		heapify(ar, cmp, largest, max);
	}
}

/** Build the heap from the given array by repeatedly invoking heapify. */
static void 
buildHeap (DATATYPE *ar, int(*cmp)(const DATATYPE,const DATATYPE), int n) 
{
	int i;
	for (i = n/2-1; i>=0; i--) {
		heapify (ar, cmp, i, n);
	}
}

/** Sort the array using Heap Sort implementation. */
void sortPointers (DATATYPE *ar, int n, 
	int(*cmp)(const DATATYPE,const DATATYPE))
{
	int i;
	buildHeap (ar, cmp, n);
	for (i = n-1; i >= 1; i--) 
	{
		swap(ar[0], ar[i]);
		heapify (ar, cmp, 0, i);
	}
}

int is_greater_than(const DATATYPE a, const DATATYPE b) 
{ 
	if(a==b)
		return 0;
	else if(a>b)
		return 1; 
	else
		return -1;
}


void assert_sorted(DATATYPE ar[], int count)
{
	for(int i=0; i<count-1; i++) {
		if(! (ar[i]<=ar[i+1]) ) {
			printf("Got error on case idx=%d!\n", g_cases);
			assert(0);
		}
	}
	g_cases++;
}

#define HEAPSORT(...) do { \
	DATATYPE ar[] = {__VA_ARGS__}; \
	sortPointers(ar, COUNT(ar), is_greater_than); \
	assert_sorted(ar, COUNT(ar)); \
} while(0)

int main()
{
	HEAPSORT(2, 1, 3); // output [1,2,3]
	HEAPSORT(3, 2, 1); // output [1,2,3]
	HEAPSORT(1,2,3,4,5); // output [1,2,3,4,5]
	HEAPSORT(5,4,3,2,1); // output [1,2,3,4,5]
	HEAPSORT(8,7,6,5,4,3,2,1);
	HEAPSORT(8,2,6,1,4,3,7,5);

	HEAPSORT(8,22, 6 ,22,4,3, 6 ,5);

	printf("All %d cases OK.\n", g_cases);
	return 0;
}

