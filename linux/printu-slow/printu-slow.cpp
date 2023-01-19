#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <iconv.h>
#include <unistd.h>

const int EACH_SLEEP_MILLISEC = 200;

extern"C++"
template <typename TElement, int N>
char (*
    RtlpNumberOf(TElement (&rparam)[N])
    )[N];
#define ARRAYSIZE(arr)    ( sizeof(*RtlpNumberOf(arr)) )

int cvt_wchar_to_utf8(const wchar_t *iwbuf, int nwchars, char *o8buf, int obufbytes,
    int *p_retbytes)
{
    const char* conv_to = "utf8", * conv_from = "utf-32";
//    char ibuf[200] = "'\xe5\xb9\xb3\xe8\xa1\x8c'", obuf[200]; // 平行

    iconv_t cd = iconv_open(conv_to, conv_from);

    if (cd == (iconv_t)-1)
    {
        printf("iconv_open(\"%s\",\"%s\") error.(errno=%d)\n",
            conv_to, conv_from, errno);
        exit(1);
    }

    char  *pibuf = (char*)iwbuf;
    size_t nibuf0 = nwchars*sizeof(wchar_t), nibuf = nibuf0;
    char  *pobuf = (char*)o8buf;
    size_t nobuf = obufbytes;
	
    int ncvt = (int)iconv(cd, &pibuf, &nibuf, &pobuf, &nobuf);
    if(ncvt<0)
    {
        printf("iconv() fail with errno=%d.", errno);
        exit(1);
    }

    int err = (int)iconv(cd, &pibuf, &nibuf, &pobuf, &nobuf);
    assert(err == 0);

    err = iconv_close(cd);
	if(err)
	{
        printf("iconv_close() fail with errno=%d.", errno);
        exit(1);
	}

    *p_retbytes = (int)(obufbytes - nobuf);
	
    return (int)( (nibuf0-nibuf) / sizeof(wchar_t) );
}

void myprint_slow(const char *s, int bytes)
{
    for (int i = 0; i < bytes; i++)
    {
        printf("%c", s[i]);
        fflush(stdout);
        usleep(EACH_SLEEP_MILLISEC * 1000);
    }
}

void myprint_wchars(char *argv[])
{
    // Each argv[x] points to a hexrpW4 word.
	
    wchar_t wbuf[400] = {};
    char utf8buf[ARRAYSIZE(wbuf) * 4] = {};

	int i = 0;
	for(; i<(int)ARRAYSIZE(wbuf); i++)
	{
        if (argv[i] == nullptr)
            break;
		
        wbuf[i] = (wchar_t)strtoul(argv[i], nullptr, 16);
	}

    int wchars = i;
    int utf8bytes = 0;
    int wcret = cvt_wchar_to_utf8(wbuf, wchars, utf8buf, sizeof(utf8buf), &utf8bytes);
    assert(wcret > 0);

    myprint_slow(utf8buf, utf8bytes);
}

bool ishexdigit(char c)
{
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        return true;
    else
        return false;
}

unsigned char hexdigit2val(char c)
{
    if (c >= '0' && c <= '9')
        return (unsigned char)(c - '0');
    else if (c >= 'a' && c <= 'f')
        return (unsigned char)(10 + (c - 'a'));
    else if (c >= 'A' && c <= 'F')
        return (unsigned char)(10 + (c - 'A'));
    else
        return 0;
}

unsigned char hexpair2byte(const char *hexpair)
{
    return (unsigned char)( hexdigit2val(hexpair[0]) * 16 + hexdigit2val(hexpair[1]) );
}

void myprint_hexrp00(char *argv[])
{
    int i = 0;
    for(; argv[i]!=nullptr; i++)
    {
        const char* hexrp00 = argv[i];

        unsigned char cbuf[100] = {};
        int j = 0;
    	for(; j<(int)sizeof(cbuf) && hexrp00[2*j]; j++)
    	{
    		if(hexrp00[2*j+1]=='\0')
    		{
                printf("[ERROR] Odd hexrp00 string digits encountered: %s\n", hexrp00);
                exit(1);
    		}

    		if(!ishexdigit(hexrp00[2*j]) || !ishexdigit(hexrp00[2*j+1]))
    		{
                printf("[ERROR] Wrong hexrp00 string digits (offset:%d) encountered in: %s\n", 
                    j*2, hexrp00);
                exit(1);
    		}

    		cbuf[j] = hexpair2byte(&hexrp00[2*j]);
    	}

        myprint_slow((const char*)cbuf, j);
    }
	
}

void print_helptext()
{
    printf("print unicode text to console, slowly, as UTF-8 stream byte-by-byte.\n");
    printf("Examples:\n"
"printu-slow 41 42 4B\n"
"printu-slow 41424B\n"
"\n"
"    will see: \"ABK\"\n"
"\n"
"printu-slow 41 42 4B e7 94 b5 e8 84 91\n"
"printu-slow 41 42 4B e794b5 e88491\n"
"\n"
"    will see: \"ABK电脑\"\n"
"\n"
"printu-slow wchar 41 42 4B 7535 8111\n"
"\n"
"    will see: \"ABK电脑\"\n"
"    If first param is 'wchar', each of remaining params represents one Unicode value, in hex.\n"
"\n"
"printu-slow wchar 41424B 7535 8111\n"
"\n"
"    will crash the program, bcz 0x41424B is not a legal Unicode codepoint.\n"
"\n"
	);
}

int main(int argc, char *argv[])
{
    if(argc==1)
    {
        print_helptext();
        exit(0);
    }

	if(strcmp(argv[1], "wchar") == 0)
	{
		if(argc==2)
		{
            printf("Nothing to print.\n");
            exit(1);
		}
        myprint_wchars(argv+2);
	}
    else
    {
        myprint_hexrp00(argv+1);
    }

    printf("\n");
	
	return 0;
}
