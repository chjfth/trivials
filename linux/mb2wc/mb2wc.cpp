#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main(int argc, char *argv[])
{
	if(argc==1)
	{
		printf("Usage: mb2wc <locale> [Hex-represented bytes ...]\n");
		printf("Example, get the Unicode value of GBK char 0xB5E7:\n");
		printf("    mb2wc zh_CN.gbk B5 E7 \n");
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
	char srcb[1000];
	int apos = 2, bpos = 0;//const char **startpp = argv+2;
	for(; apos<argc; apos++,bpos++)
	{
		srcb[bpos] = strtoul(argv[apos], NULL, 16);
	}
	
	int wc_chars = 0, ill_chars = 0;
	mbstate_t mbst = {0};
	for(bpos=0; bpos<srcn; wc_chars++)
	{
		wchar_t wc = 0;
		int mblen = mbrtowc(&wc, srcb+bpos, srcn-bpos, &mbst);
		if(mblen==-1) // illegal char-seq
		{
			printf("[%d]ill ", bpos);
			bpos++; // skip the illegal char and try from next byte
			ill_chars++;
		}
		else if(mblen==-2)
		{
			printf("[%d]Incomplete MBCS at end", bpos);
			ill_chars++;
			wc_chars++;
			break;
		}
		else if(mblen==0) // NULL char encountered
		{
			printf("[%d]0 ", bpos);
			bpos++;
		}
		else // good result
		{
			printf("[%d]%02X ", bpos, wc);
			bpos += mblen;
		}
	}

	printf("\nTotal wchars %d, %d of which ill.\n", wc_chars, ill_chars);

	return 0;
}
