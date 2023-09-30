#include <stdio.h>
#include <stdlib.h>
#include "unp_autofree.h"

/* This program shows *wrap* a resource pointer(FILE*) inside a unique_ptr object,
so that the resource can be destroyed/free-ed on current function exit.
*/

int fclose_debug_wrapper(FILE *fp)
{
	printf("[DBG] fclose(%p)\n", fp);
	return fclose(fp);
}

MakeUniquePtrClass(Unp_fclose, fclose_debug_wrapper, FILE*)

void test_autofree(const char *filename)
{
	FILE *fp = fopen("abc1.txt", "w");
	if(!fp) {
		printf("%s open fail!\n", filename);
		return;
	}

	// Ensure fclose_debug_wrapper(fp) is called on current function exit.
	Unp_fclose _unp_fp(fp); 

	printf("%s opened, FILE* ptr: %p\n", filename, fp);
}

int main(int argc, char* argv[])
{
	printf("Hello, unp_autofree!\n");

	test_autofree("abc1.txt");
	
	return 0;
}
