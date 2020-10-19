#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>

#include <ps_TCHAR.h>
#include <ps_TypeDecl.h>
#include <commdefs.h>
#include <EnsureClnup_common.h>
#include <getopt/sgetopt.h>
#include <mm_snprintf.h>

#include <linux/termios_util.h>
#include "utils.h"

MakeCleanupClass(Cec_LibcFh, int, close, int, -1)

char g_szdev[256];

int g_bytes_per_write_init = 0;
int g_start_seq = 0;

bool g_is_nonblock = false;
bool g_is_restore_tio = true;

bool g_is_tcdrain_before_close = false;

int g_baudrate = 9600;

int g_vmin = 1; // 1 is the system default for /dev/ttyS0, /dev/ttyACM0 
int g_vtime = 0;

int g_hexdump_columns = -1; // -1 means no print, 0 means print sequentially

static int s_count = 0;

const int MaxInts = 1024000;
Uint g_sendbuf[MaxInts];

const int MaxRbuf = 128000;
char g_readbuf[MaxRbuf];

enum writeop_et 
{
	write_only = 0,
	select_and_write = 1,
	poll_and_write = 2,
	read_once = 3,
};

writeop_et g_op_init = write_only;

enum 
{
	OPCHAR_W = 'w',
	OPCHAR_S = 's',
	OPCHAR_P = 'p',
	OPCHAR_total = 't',
	OPCHAR_read = 'r',
};

static const TCHAR *app_short_options = _T("b:B:");

enum {
	optn_op = 128,
	optn_nonblock, 
	optn_vmin,     // terminal device VMIN param
	optn_vtime,    // terminal device VTIME param
	optn_rhexdump, // hexdump read bytes
	optn_peek_sig,
	optn_no_restore_termios,
	optn_tcdrain_before_close,
};

static sgetopt_option app_long_options [] = 
{
	{_T("op"), has_arg_yes, 0, optn_op},
	{_T("bytes-per-write"), has_arg_yes, 0, 'b'},
	{_T("baudrate"), has_arg_yes, 0, 'B'},
	{_T("nonblock"), has_arg_no, 0, optn_nonblock},
	{_T("vmin"), has_arg_yes, 0, optn_vmin},
	{_T("vtime"), has_arg_yes, 0, optn_vtime},
	{_T("hexdump"), has_arg_yes, 0, optn_rhexdump},
	{_T("peek-sig"), has_arg_no, 0, optn_peek_sig},
	{_T("no-restore-termios"), has_arg_no, 0, optn_no_restore_termios},
	{_T("tcdrain-before-close"), has_arg_no, 0, optn_tcdrain_before_close},
	{0}
};

void parse_args(int argc, char **argv)
{
	sgetopt_ctx *si = sgetopt_ctx_create(); // important 

	while(1)
	{
		int longindex = 0;
		int optc = sgetopt_long(si, argc, argv,
			app_short_options, app_long_options, &longindex);
		
		if (optc == -1)
			break;

		if (optc == '?')
		{
			// Chj: Short-option error and Long-option error are determined differently.
			if(si->optopt)
			{
				// Short-option error
				if(sgetopt_is_valid_short(si->optopt, app_short_options))
					mm_printf(_T("Short option '-%c' missing an argument.\n"), si->optopt);
				else
					mm_printf(_T("Bad short option '-%c'.\n"), si->optopt);

				si->optopt = 0; // otherwise, the err will still be there when next error is on a long-option
			}
			else
			{
				// Long-option error
				const TCHAR *problem_opt = argv[si->optind - 1];
				if (sgetopt_is_valid_long(problem_opt, app_long_options))
					mm_printf(_T("Long option '%s' missing an argument\n"), problem_opt); 
				else
					mm_printf(_T("Bad long option '%s'\n"), problem_opt); 
			}
			exit(2); // continue; 
		}

		const TCHAR *optarg = si->optarg;
		int argval = optarg ? atoi(optarg) : 0;

		switch (optc)
		{{
		case optn_op:
		{	
			int opchar = optarg[0];
			if (opchar == OPCHAR_read)
				g_op_init = read_once;
			else if (opchar == OPCHAR_S)
				g_op_init = select_and_write;
			else if (opchar == OPCHAR_P)
				g_op_init = poll_and_write;
			else
				g_op_init = write_only;
			break;
		}	
		case 'b':
			g_bytes_per_write_init = argval;
			break;

		case 'B':
			g_baudrate = argval;
			break;
			
		case optn_nonblock:
			g_is_nonblock = true;
			break;
			
		case optn_vmin:
			g_vmin = argval;
			break;
		case optn_vtime:
			g_vtime = argval;
			break;
			
		case optn_rhexdump:
			g_hexdump_columns = argval;
			break;
			
		case optn_peek_sig:
			g_is_peek_SIGINT = true;
			break;
			
		case optn_no_restore_termios:
			g_is_restore_tio = false;
			break;
			
		case optn_tcdrain_before_close:
			g_is_tcdrain_before_close = true;
			break;

		default:
			mm_printf("You missed some option-processing.\n");
			assert(0);
		}}
	
	}

	if (si->optind < argc)
	{
		strncpy(g_szdev, argv[si->optind], GetEleQuan(g_szdev));
	}
	else
	{
		mm_printf("Device name parameter missing.\n");
		exit(2);
	}

	sgetopt_ctx_delete(si);
}

char Op2Char(writeop_et op)
{
	if (op == write_only)
		return 'w';
	else if (op == select_and_write)
		return 's';
	else if (op == poll_and_write)	
		return 'p';
	else if (op == read_once)
		return 'r';
	else
		return '?';
}

const char *fill_wrbuf(int offset, int bytes)
{
	assert(offset >= 0);
	
	// The wrbuf content sample:
	// If offset==0,
	// 00 00 00 00,00 00 00 01,00 00 00 02,00 00 00 03,....
	//
	// If offset==16,
	// 00 00 00 04,00 00 00 05,00 00 00 06,00 00 00 07,....
	//
	// If offset==19,
	// 04,00 00 00 05,00 00 00 06,00 00 00 07,00 00 00 08,....
	
	int ofsInt = offset / sizeof(int);
	int nInts = bytes / sizeof(int) + 1;
	
	for (int i = 0; i < nInts; i++)
	{
		// We use big-endian order for easier human analysis.
		Uint u = ofsInt + i;
		Uint *ibuf = g_sendbuf;
		
		Uchar *p = (Uchar*)(ibuf+i);
		p[0] = (u >> 24) & 0xff; 
		p[1] = (u >> 16) & 0xff;
		p[2] = (u >> 8) & 0xff;  
		p[3] = (u) & 0xff;
	}
	
	return (char *)g_sendbuf + offset%sizeof(int);
}


int select_for_write(int fd, int timeout)
{
	fd_set fd_write;
	FD_ZERO(&fd_write);
	FD_SET(fd, &fd_write);
	
//	timeval tv_write = {0, 0};
//	if (timeout > 0)
//	{
//		tv_write.tv_sec = timeout / 1000;
//		tv_write.tv_usec = timeout % 1000 * 1000;
//	}
	timespec ts_write = {0, 0};
	if (timeout > 0)
	{
		ts_write.tv_sec = timeout / 1000;
		ts_write.tv_nsec = timeout % 1000 * (1000 * 1000);
	}	

	sigset_t sigset_allow_SIGINT;
	sigprocmask(0, NULL, &sigset_allow_SIGINT); // query orig sigmask
	sigdelset(&sigset_allow_SIGINT, SIGINT); // remove SIGINT from mask
	
	int nAvai = pselect(fd+1, NULL, &fd_write, NULL, 
		timeout >= 0 ? &ts_write : NULL,
		&sigset_allow_SIGINT);

	return nAvai;
}

#if 0
void test_select_write()
{
	Cec_LibcFh hdev = open("1.txt", O_RDWR | O_NOCTTY | O_NONBLOCK);
	char c = 'E';
	int nAvai = select_for_write(hdev, -1);
	int nwr = write(hdev, &c, 1);
	printf("nwr=%d, errno=%d\n", nwr, errno); // Got nwr=-1, errno=9 (EBADF)
}
#endif

int poll_for_write(int fd, int timeout)
{
	timespec ts_write = { 0, 0 };
	if (timeout > 0)
	{
		ts_write.tv_sec = timeout / 1000;
		ts_write.tv_nsec = timeout % 1000 * (1000*1000);
	}
	
	sigset_t sigset_allow_SIGINT;
	sigprocmask(0, NULL, &sigset_allow_SIGINT); // query orig sigmask
	sigdelset(&sigset_allow_SIGINT, SIGINT); // remove SIGINT from mask
	
	pollfd pollo = {fd, POLLOUT};
	int nAvai = ppoll(&pollo, 1, 
		timeout >= 0 ? &ts_write : NULL,
		&sigset_allow_SIGINT);

	return nAvai;
}

bool do_write_once(int hdev, int bytes_to_write, writeop_et op, int *p_totwr)
{
	int init_total = *p_totwr;
	
	mm_printf("<#%d>\n", s_count++);
	
	if (op == select_and_write)
	{
		PrintTs("select() for write...");
		
		int nAvai = select_for_write(hdev, -1);
		if (nAvai < 0)
		{
			PrintTsErr("select_for_write() fails.");
			return false;
		}
		else if (nAvai == 0)
		{
			PrintTs("select_for_write() timeout.");
			return true;			
		}
	}
	else if (op == poll_and_write)
	{
		PrintTs("poll() for write...");
		
		int nAvai = poll_for_write(hdev, -1);
		if (nAvai < 0)
		{
			PrintTsErr("poll_for_write() fails.");
			return false;
		}
		else if (nAvai == 0)
		{
			PrintTs("poll_for_write() timeout.");
			return true;			
		}
	}

	const char *wrbuf = fill_wrbuf(init_total, bytes_to_write);
			
	PrintTs("write() %d bytes...", bytes_to_write);
		
	BlockSignal(SIGINT, false);

	int nwr = write(hdev, wrbuf, bytes_to_write);
	
	BlockSignal(SIGINT, true);
	
	if (nwr >= 0)
	{
		PrintTs("write() returns %d %s", nwr, nwr<bytes_to_write?"(less)":"");
		
		*p_totwr += nwr;
		return true;
	}
	else
	{
		PrintTsErr("write() fails.");
		return false;
	}
	
}

int do_read_once(int hdev, int bytes_to_read)
{
	mm_printf("<#%d>\n", s_count++);
	
	PrintTs("Doing read() %d bytes...", bytes_to_read);
		
	BlockSignal(SIGINT, false);

	int red = read(hdev, g_readbuf, bytes_to_read);
	
	BlockSignal(SIGINT, true);
	
	if (red >= 0)
	{
		PrintTs("read() returns %d %s", red, red<bytes_to_read?"<less>":"");
		
		if (g_hexdump_columns == 0)
			mm_printf("%k%*m\n", " ", red, g_readbuf);
		else if (g_hexdump_columns > 0) 
			mm_printf("%4R%k%*m", g_hexdump_columns, " ", red, g_readbuf);
		
		return red;
	}
	else
	{
		PrintTsErr("read() fails.");
		return -1;
	}
}

RawTermError_ret 
termios_SetVminVtime(int fd, int vmin, int vtime, termios *pTioOrig)
{
	RawTermError_ret ret = RTE_Unknown;
	int ret_errno = 0;
	bool succ = false;
	termios tio_orig = {};
	if (!pTioOrig)
		pTioOrig = &tio_orig;
	memset(pTioOrig, 0, sizeof(termios));
	
	if (!isatty(fd))
		return RTE_NotTTY;

	if(! (vmin>=0 && vmin<=255 && vtime>=0 && vtime<=255) )
		return RTE_BadParam;
	
	int err = tcgetattr(fd, pTioOrig);
	if (err)
		return RTE_Unknown;
	
	termios tio = *pTioOrig;
	tio.c_cc[VMIN] = vmin;
	tio.c_cc[VTIME] = vtime;
	
	err = tcsetattr(fd, TCSANOW, &tio);
	if (err)
		return RTE_Unknown;
	
	return RTE_Success;
}

int do_test(Fhandle_t hdev)
{
	int err = 0;
	
	RawTermError_ret rte = termios_SetRawTerm(hdev, g_baudrate);
	if (rte!=RTE_Success)
	{
		printf("Set terminal raw mode fail(%s).\n", RTE2Name(rte));
		return FAIL__1;
	}
	
	rte = termios_SetVminVtime(hdev, g_vmin, g_vtime, NULL);
	if (rte!=RTE_Success)
	{
		printf("Set terminal VMIN VTIME error(%s).\n", RTE2Name(rte));
		return FAIL__1;
	}

	int pbsize = (int)fpathconf(hdev, _PC_PIPE_BUF);
	if (pbsize > 0)
	{
		mm_printf("fpathconf(hdev, _PC_PIPE_BUF) returns %d bytes.\n", pbsize);
	}
	
//	err = lockf(hdev, F_TLOCK, 0); // this is only co-operative lock, so don't use
//	if (err){
//		mm_printf("F_TLOCK the device fail. errno=%d\n", errno);
//		return hdev;
//	}
	
	if(g_bytes_per_write_init==0)
	{	
		g_bytes_per_write_init = 100;
		PrintCmdHelp();
	}
	
	char ans[80];
	int bytes_to_rw = g_bytes_per_write_init;
	int totwritten = 0, totred = 0;
	writeop_et op = g_op_init;
	
	for (;;)
	{
		mm_printf("Choose operation(%c=%d.B): ", Op2Char(op), bytes_to_rw);
		fflush(stdout);
		fgets(ans, sizeof(ans), stdin);
		char key = ans[0];
		if (key == 'q')	{
			break;
		}
		else if (key == '\r' || key == '\n') {
			// user press enter directly, execute same op as prev.
		}
		else if (key == OPCHAR_W)
			op = write_only;	
		else if (key == OPCHAR_S)
			op = select_and_write;
		else if (key == OPCHAR_P)
			op = poll_and_write;
		else if (key == OPCHAR_total)
		{
			mm_printf("Total read bytes: %d, Total written bytes: %d\n", totred, totwritten);
			printf("\n");
			continue;
		}
		else if (key == OPCHAR_read)
			op = read_once;
		else
		{
			// consider it a number	
			int ans_bytes = strtoul(ans, NULL, 0);
			if (ans_bytes > 0)
			{
				bytes_to_rw = ans_bytes;
			}
			continue;
		}

		if (op == read_once)
		{
			int nred = do_read_once(hdev, bytes_to_rw);
			if (nred > 0)
				totred += nred;
		}
		else //  various write op
		{
			do_write_once(hdev, bytes_to_rw, op, &totwritten);
		}

		printf("\n");
	}

//	lockf(hdev, F_ULOCK, 0);
	return NOERROR_0;
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("tiowrite v1.0, compiled on %s %s\n", __DATE__, __TIME__);
		printf("You should at least pass in a device name to operate.\n");
		printf("For example:\n");
		printf("  tiowrite /dev/ttyS0\n");
		printf("  tiowrite /dev/ttyGS0\n");
		printf("  tiowrite /dev/ttyACM0\n");
		printf("\n");
		printf("Options:\n");
		printf("  --nonblock: Do nonblocking write.\n");
		printf("  -b 4096: Initial write bytes set to 4096.\n");
		printf("  -B <baudrate> or --baudrate <baudrate>\n");
		printf("  --vmin=<m> --vtime=<t> (m and t should be 0-255)\n");
		printf("  --hexdump=0 or --hexdump=16 : hex dump read bytes\n");
		printf("  --no-restore-termios\n");
		printf("  --tcdrain-before-close\n");
		printf("  --op=<c> : Initial op, <c> can be: r w s p\n");
		printf("  --peek-sig : print SIGINT count on quit\n");
		exit(1);
	}

	parse_args(argc, argv);

	//	test_select_write();

	// This program is designed to have the following behavior:
	// User can press Ctrl+C from the terminal to break(stop) a select/poll/write
	// operation. Ctrl+C is not a way to force program quit.
	// To force program quit, user can press Ctrl+\ .

	if (!prepare_SIGINT())
		exit(1);

	int err = 0;
	int hdev = open(g_szdev, 
		O_RDWR | O_NOCTTY | (g_is_nonblock?O_NONBLOCK:0)
		);
	
	if(hdev==-1)
	{
		PrintErr(errno, "open('%s') fails.", g_szdev);
		return -1;
	}
	
	mm_printf("open('%s') success. Device-handle=0x%X %s, isatty()=%s\n", 
		g_szdev, 
		(int)hdev, g_is_nonblock?"(nonblock)":"(blocking)",
	    isatty(hdev) ? "yes" : "no"
		);
	
	struct termios tio_orig = {};
	if (g_is_restore_tio && isatty(hdev))
	{	
		// Save original termios
		err = tcgetattr(hdev, &tio_orig);
		assert(!err);
	}
	
	do_test(hdev); // The big work

	// Allow SIGINT now, so that it can break tcdrain(), tcsetattr() and close(),
	// in case these functions blocks waiting for Wbuf draining.
	BlockSignal(SIGINT, false); 
	
	int err_tcdrain = 0;
	if(g_is_tcdrain_before_close)
	{
		PrintTs("Calling tcdrain(), waiting for Wbuf to drain...");
		
		// note: tcdrain() will return errno=EINTR on SIGINT.
		// For writing to /dev/ttyACM0, on down-stream jam, we have to use SIGINT
		// to force tcdrain()'s return.
		err_tcdrain = tcdrain(hdev);
		
		PrintTsErr("tcdrain() done.");
	}
	
	if (g_is_restore_tio && isatty(hdev))
	{
		// Restore original termios
		PrintTs("Restoring termios with tcsetattr(%s)...",
			err_tcdrain ? "TCSANOW" : "TCSAFLUSH");
		
		err = tcsetattr(hdev, 
			err_tcdrain ? TCSANOW : TCSAFLUSH, 
		    &tio_orig);
		
		if(!err)
			PrintTsErr("Restoring termios success.");
		else
			PrintTsErr("Restoring termios fail."); // may got EINTR
	}

	PrintTs("close() device handle...");
	err = close(hdev);
	if (!err)
		PrintTs("close() success.");
	else
		PrintTsErr("close() fail.");

	report_SIGINT_count();

//	fpathconf(fdpipe, _PC_PIPE_BUF);
	
	return 0;
}

