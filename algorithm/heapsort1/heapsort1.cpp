// Origin: $\algorithms-nutshell-2ed\Code\Sorting\PointerBased\straight_HeapSort.c

#include <stdio.h>

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

/** Heapify the subarray ar[0,max). */
static void heapify (DATATYPE *ar, 
	int(*cmp)(const DATATYPE,const DATATYPE),
	int idx, int max) 
{
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

#define COUNT(ar) (sizeof(ar)/sizeof(ar[0]))

#define HEAPSORT(ar) sortPointers(ar, COUNT(ar), is_greater_than)

int main()
{
	int a1[] = {2, 1, 3};
	HEAPSORT(a1); // output [1,2,3]

	int a2[] = {3, 2, 1};
	HEAPSORT(a2); // output [1,2,3]

	int a3[] = {1,2,3,4,5};
	HEAPSORT(a3); // output [1,2,3,4,5]

	int a4[] = {5,4,3,2,1};
	HEAPSORT(a4); // output [1,2,3,4,5]

	return 0;
}

