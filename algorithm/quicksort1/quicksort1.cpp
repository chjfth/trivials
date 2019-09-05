// Origin: $\algorithms-nutshell-2ed\Code\Sorting\PointerBased\baseQsort.c

#include <assert.h>

/** Problem size below which to use insertion sort. */
extern int minSize;
int minSize = 0; // if 0, no short insertion-sort will be called.

#define COUNT(ar) (sizeof(ar)/sizeof(ar[0]))
int g_cases = 0;
int g_cmps = 0;
int g_swaps = 0;
int g_copies = 0; // only in insertion sort

typedef int DATATYPE;

/** Code to select a pivot index around which to partition ar[left, right]. */
int selectPivotIndex (DATATYPE *vals, int left, int right)
{
	return right;
}


/**
 * @file baseQsort.c   Quicksort implementation
 * @brief 
 *   Complete Quicksort implementation optimized to switch to Insertion 
 *   Sort when problem sizes are less than or equal to minSize in length.
 *   For pure QuickSort, set minSize to 0.
 *   
 * 
 * @author George Heineman
 * @date 6/15/08
 */

#include <stdio.h>
#include <sys/types.h>

//#include "report.h"

void swap(DATATYPE &a, DATATYPE &b)
{
	DATATYPE tmp = a; a = b; b = tmp;
	g_swaps++;
}

/**
 * In linear time, group the subarray ar[left, right] around a pivot
 * element pivot=ar[pivotIndex] by storing pivot into its proper location,
 * store, within the sub-array (whose location is returned by this
 * function) and ensuring that all ar[left,store) <= pivot and all
 * ar[store+1,right] > pivot.
 * 
 * @param ar           array of elements to be sorted.
 * @param cmp          comparison function to order elements.
 * @param left         lower bound index position  (inclusive)    
 * @param right        upper bound index position  (inclusive)
 * @param pivotIndex   index around which the partition is being made.
 * @return             location of the pivot index properly positioned.
 */


int partition (DATATYPE *ar, 
	int(*cmp)(const DATATYPE,const DATATYPE),
	int left, int right, int pivotIndex) 
{
	int idx, store;

	DATATYPE pivot = ar[pivotIndex];

	/*  move pivot to the end of the array*/
	swap(ar[pivotIndex], ar[right]);
  
	/* all values <= pivot are moved to front of array and pivot inserted
	* just after them. */
	store = left;
	for (idx = left; idx < right; idx++) 
	{
		if (cmp(ar[idx], pivot) <= 0) 
		{
			if(idx!=store) // chj optimize
				swap(ar[idx], ar[store]);

			store++;
		}
	}
  
	swap(ar[right], ar[store]);
	return store;
}

/**  proper insertion sort, optimized */
void insertion (DATATYPE *ar, 
	int(*cmp)(const DATATYPE,const DATATYPE),
	int low, int high) 
{
	int loc;
	for (loc = low+1; loc <= high; loc++) 
	{
		int i = loc-1;
		DATATYPE value = ar[loc];
		
		while (i >= 0 && cmp(ar[i], value)> 0) 
		{
			ar[i+1] = ar[i];
			i--;
			g_copies++;
		} 

		ar[i+1] = value;
	}
}

/** Implement SelectionSort if one wants to see timing difference 
  if this is used instead of Insertion Sort. */
void selectionSort (DATATYPE *ar, 
	int(*cmp)(const DATATYPE,const DATATYPE),
	int low, int high) 
{
	int t, i;

	if (high <= low) { 
		return; 
	}

	for (t = low; t < high; t++) 
	{
		for (i = t+1; i <= high; i++) 
		{
			if (cmp(ar[i], ar[t]) <= 0) 
			{
				swap(ar[t], ar[i]);
			}
		}
	}
}

/**
 * Sort array ar[left,right] using Quicksort method.
 * The comparison function, cmp, is needed to properly compare elements.
 */
void do_qsort (DATATYPE *ar, 
	int(*cmp)(const DATATYPE, const DATATYPE),
	int left, int right) 
{
	if (right <= left) { 
		return; 
	}

	/* partition */
	int pivotIndex = selectPivotIndex (ar, left, right);
	pivotIndex = partition (ar, cmp, left, right, pivotIndex);

	if (pivotIndex-1-left <= minSize) {
		insertion (ar, cmp, left, pivotIndex-1);
	} else {
		do_qsort (ar, cmp, left, pivotIndex-1);
	}

	if (right-pivotIndex-1 <= minSize) {
		insertion (ar, cmp, pivotIndex+1, right);
	} else {
		do_qsort (ar, cmp, pivotIndex+1, right);
	}
}

/** Sort by using Quicksort. */
void
sortPointers (DATATYPE *vals, int total_elems, 
	int(*cmp)(const DATATYPE,const DATATYPE)) 
{
	do_qsort (vals, cmp, 0, total_elems-1);
}

/* debugging method. */
void output (char **ar, int n) {
  int i;
  for (i = 0; i < n; i++) {
    printf ("%d. %s\n", i, ar[i]);
  }
}

//////////////////////////////////////////////////////////////////////////

int is_greater_than(const DATATYPE a, const DATATYPE b) 
{
	g_cmps++;
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
}

void sort_and_verify(DATATYPE ar[], int count, 
	int(*cmp)(const DATATYPE,const DATATYPE))
{
	printf("[#%d] Sorting %d elements... ", g_cases, count);
	fflush(stdout);

	int ocmps=g_cmps, oswaps=g_swaps, ocopies=g_copies;

	sortPointers(ar, count, cmp);
	assert_sorted(ar, count); 

	int cmps = g_cmps-ocmps;
	int swaps = g_swaps-oswaps;
	int copies = g_copies-ocopies;
	int total = cmps+swaps+copies;
	printf("Done. cmps %d , swaps %d , copies %d (total %d).\n", 
		cmps, swaps, copies, total);

	g_cases++;
}

#define SORT(...) do { \
	DATATYPE ar[] = {__VA_ARGS__}; \
	sort_and_verify(ar, COUNT(ar), is_greater_than); \
} while(0)

int run_tests()	
{
	printf("Running quicksort cases. (ins-minsize=%d)\n", minSize);

	SORT(2, 1, 3); // output [1,2,3]
	SORT(3, 2, 1); // output [1,2,3]
	SORT(1,2,3,4,5); // output [1,2,3,4,5]
	SORT(5,4,3,2,1); // output [1,2,3,4,5]

	SORT(1,2,3,4,5,6,7,8);
	SORT(8,7,6,5,4,3,2,1);
	SORT(8,2,6,1,4,3,7,5);
	SORT(8,22, 6 ,22,4,3, 6 ,5);

	SORT(1,2,3,4,5,6,7,8,9,10);
	SORT(10,9,8,7,6,5,4,3,2,1);
	SORT(3,5,7,8,10,1,6,9,4,2);

	printf("All %d cases OK.\n", g_cases);
	return 0;
}

int main()
{
	const int minSize_start = 0;
	const int minSize_end = 5;
	for(int ms=minSize_start; ms<=minSize_end; ms++) {
		minSize = ms;
		run_tests();
		printf("\n");
	}

	return 0;
}
