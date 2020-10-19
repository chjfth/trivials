#include <limits.h> // MB_LEN_MAX
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main(int argc, char *argv[])
{
	if(argc==1)
	{
		printf("Usage: wc2mb <locale> [Hex-represented wchars ...]\n");
		printf("Example, get the GBK value of Unicode char sequence 0x7535 0x96FB:\n");
		printf("    wc2mb zh_CN.gbk 7535 96FB \n");
		return 1;
	}

	char *retlc = setlocale(LC_CTYPE, argv[1]);
	if(retlc==NULL)
	{
		printf("Invalid locale given.\n");
		return 1;
	}

	printf("setlocale(LC_CTYPE,\"%s\")=%s , MB_CUR_MAX=%d.\n", argv[1], 
		retlc, MB_CUR_MAX);

	int srcn = argc - 2;

	// prepare source mbcs bytes array
	wchar_t srcw[1000]; // source wide chars
	int apos = 2, wpos = 0;
	for(; apos<argc; apos++,wpos++)
	{
		srcw[wpos] = strtoul(argv[apos], NULL, 16);
	}
	
	int mbcs_bytes = 0; // resulting total MBCS bytes
	int ill_chars = 0; // wchars failed to convert to MBCS
	mbstate_t mbst = {0};
	for(wpos=0; wpos<srcn; wpos++)
	{
		char mbcs[MB_LEN_MAX] = {0};
		int mblen = wcrtomb(mbcs, srcw[wpos], &mbst); // overide the glibc function named `mblen'
		if(mblen==-1) // illegal char-seq
		{
			printf("[%d]ill ", wpos);
			mblen = 1;
			ill_chars++;
		}
		else if(mblen==0) // NULL char encountered
		{
			printf("[%d]Unexpected! wcrtomb should not return 0.", wpos);
			return 2;
		}
		else // good result
		{
			printf("[%d]", wpos);
			for(int i=0; i<mblen; i++)
				printf("%02X ", (unsigned char)mbcs[i]);
		}
		mbcs_bytes += mblen;
	}

	printf("\nTotal wchars %d(%d of which ill) converted to %d MBCS bytes.\n", 
		srcn, ill_chars, mbcs_bytes);

	return 0;
}
