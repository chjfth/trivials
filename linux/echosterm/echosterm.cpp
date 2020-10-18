#include "echosterm.h"

#define VER_STR "1.1"

MakeCleanupClass(Cec_LibcFh, int, close, int, -1)

char g_szdev[256]; // terminal device path

//int g_bytes_per_write_init = 0;
//int g_start_seq = 0;

//bool g_is_nonblock = false;
//bool g_is_restore_tio = true;

//bool g_is_tcdrain_before_close = false;

int g_baudrate = 9600;

int g_echobufsize = 64000;

bool g_use_epoll = false;

static const TCHAR *app_short_options = _T("b:B:");

enum {
	optn_echobufsize = 128,
	optn_use_epoll,
};

static sgetopt_option app_long_options [] = 
{
	{_T("bufsize"), has_arg_yes, 0, optn_echobufsize},
	{_T("baudrate"), has_arg_yes, 0, 'B'},
	{_T("epoll"), has_arg_no, 0, optn_use_epoll},
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
		case optn_echobufsize:
		{	
			if (argval <= 0)
				ErrExit("Invalid --bufsize value: %d", argval);
			
			g_echobufsize = argval;
			break;
		}	

		case 'B':
			if (argval <= 0)
				ErrExit("Invalid --baudrate value: %d", argval);
			g_baudrate = argval;
				
			break;
		
		case optn_use_epoll:
			g_use_epoll = true;
			break;
			
		default:
			ErrExit("BUG: This program missed some option-processing.");
		}}
	}

	if (si->optind < argc)
	{
		strncpy(g_szdev, argv[si->optind], GetEleQuan(g_szdev));
	}
	else
	{
		ErrExit("ERROR: Device name missing(/dev/ttyS0 etc).\n");
	}

	sgetopt_ctx_delete(si);
}

bool IsQuit()
{
	if (g_isquit)
	{
		PrintTs("Got quit request(Ctrl+C) from user.");
		return true;
	}
	else
		return false;
}

int s1_ReadHunk(int fd, char *buf, int bufsize, CCalSpeed &cs)
{
	pollfd pi = {fd, POLLIN};
	
	// wait infinitely for first piece of data
	int nAvai = poll(&pi, 1, -1); 
	if (nAvai == -1 && errno == EINTR)
		return -1; // user wants to quit.
	
	assert(nAvai == 1);
	
	int nRed = read(fd, buf, bufsize);
	if (nRed == 0)
	{
		printf("read() meets end-of-file.\n");
		return -1;
	}
	else if (nRed < 0)
	{
		PrintErr(errno, "read() meets error.");
		return -1;
	}
	
	cs.Accum(nRed); // nRed may be less than bufsize
		
	return nRed;
}

int s1_WriteHunk(int fd, const char *buf, int wbytes)
{
	pollfd pi = {fd, POLLOUT}; // special note: POLL_OUT is a different value!
	
	int nTotWtn = 0;
	for (;;)
	{
		int nAvai = poll(&pi, 1, -1);
		if (nAvai == -1 && errno == EINTR)
			return -1; // user wants to quit.
	
		assert(nAvai == 1);

		int remain = wbytes - nTotWtn;
		int nWtn = write(fd, buf+nTotWtn, remain);
		assert(nWtn != 0);
		if (nWtn <= 0)
		{
			PrintErr(errno, "write() meets error.");
			return -1;
		}
		
		nTotWtn += nWtn;
		if (nTotWtn == wbytes)
			break;
	}
	
	return nTotWtn;
}

void start_EchoServer_simple(int fd, int bufsize)
{
	Cec_delete_pchar cec_buf = new char[bufsize];
	char *echobuf = cec_buf;
	
	CCalSpeed cs(GetOsMillisec32);
	Uint diffmsec=0; Uint64 diffbytes=0;
	char szspeed[16];
	
	int nWaterHigh = 0;
	Uint64 nTotRed64 = 0;
	for (;;)
	{
		int nRed = s1_ReadHunk(fd, echobuf, bufsize, cs);
		if (nRed <= 0)
			break;
		
		if (cs.GetNextSpeed(1000, &diffmsec, &diffbytes))
		{
			CCalSpeed::sprint(diffmsec, diffbytes, szspeed, GetEleQuan(szspeed));
			
			PrintTs("Recv total: %T%llU Echo speed: %s", 
			        ",", nTotRed64, szspeed);

			if (nRed > nWaterHigh)
			{
				nWaterHigh = nRed;
				printf("Echobuf new water high mark: %d bytes.\n", nWaterHigh);
			}
		 }
		
		int nWtn = s1_WriteHunk(fd, echobuf, nRed);
		if (nWtn != nRed)
			break;
		
		if (IsQuit())
			break;
	}
	
	printf("Echobuf water mark highest: %d bytes.\n", nWaterHigh);
}

void start_EchoServer_epoll(int fd, int bufsize)
{
	const int shadow_size = _MIN_(bufsize, 16384);
	
	int nRed = 0, nWtn = 0;
	
	CCalSpeed cs(GetOsMillisec32);
	Uint diffmsec = 0; Uint64 diffbytes = 0;
	Uint count_EAGAIN = 0, count_EAGAIN_prev = 0;
	char szspeed[16];
	
	CCircBufws circbuf;
	CCircBufws::ReCode_et cberr = circbuf.Init(bufsize, shadow_size);
	assert(!cberr);
	
	int epollfd = epoll_create(2);
	if (epollfd == -1)
	{
		PrintTsErr("epoll_create() fail.");
		return;
	}
	
	struct epoll_event ev = {}, evret = { }; //, events[2] = {};
	ev.events = EPOLLIN|EPOLLET; // |EPOLLOUT;
	ev.data.fd = fd;
	
	int err = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	assert(!err);
	
	int nWaterHigh = 0, nWaterHigh_prev = 0;
	Uint64 nTotRed64 = 0;
	for (;;)
	{
		assert(ev.events != 0);
		evret.events = 0; // optional, only for safety
		
		uint32_t ev0 = ev.events; // save it for compare later
		
		int nAvai = epoll_wait(epollfd, &evret, 1, -1);
		if (nAvai<=0)
		{
			if (errno == EINTR)
				PrintTs("epoll_wait() got EINTR, quit request from user.");
			else
				PrintTsErr("epoll_wait() returns error.");
			break;
		}
		
		if (evret.events & EPOLLIN)
		{
			int vacant = circbuf.VacantEles();
			int nToRd = _MIN_(vacant, shadow_size);
			
			void *pTail_ = circbuf.TailPtr_();
			nRed = read(fd, pTail_, nToRd);
			
			if (nRed <= 0)
			{
				if (nRed == 0)
					PrintTs("read() meets end-of-file.");
				else
					PrintTsErr("read() returns error.");
				break;
			}
			
			nTotRed64 += nRed;
			cs.Accum(nRed);
			
			int nstock = circbuf.StockEles();
			if (nstock > nWaterHigh)
				nWaterHigh = nstock;
			
			cberr = circbuf.ShadowRetrieveEnd(nRed);
			assert(!cberr);
			
			ev.events |= EPOLLOUT;
			
			// If buffer is full, stop polling for readable
			if (nRed == vacant)
				ev.events &= ~EPOLLIN;

			// check speed and print some info
			//
			if(cs.GetNextSpeed(1000, &diffmsec, &diffbytes))
			{
				CCalSpeed::sprint(diffmsec, diffbytes, szspeed, GetEleQuan(szspeed));
			
				PrintTs("Recv total: %T%llU Echo speed: %s", 
					",", nTotRed64, szspeed);

				if (nWaterHigh > nWaterHigh_prev)
				{
					nWaterHigh_prev = nWaterHigh;
					printf("Echobuf new water high mark: %d bytes.\n", nWaterHigh);
				}
				
				if (count_EAGAIN > count_EAGAIN_prev)
				{
					count_EAGAIN_prev = count_EAGAIN;
					printf("write() encounters EAGAIN %u times.\n", count_EAGAIN);
				}
			}
		}
		
		if (evret.events & EPOLLOUT)
		{
			int nstock = circbuf.StockEles();
			assert(nstock > 0);
			
			int nToWr = _MIN_(nstock, shadow_size);
			
			void *pHead = NULL;
			circbuf.ShadowPeekHead(nToWr, &pHead);
			
			nWtn = write(fd, pHead, nToWr);
			if (nWtn <= 0)
			{
				if (nWtn == 0)
					PrintTs("write() returns 0, unexpected.");
				else if (errno == EAGAIN)
				{
					count_EAGAIN++;
					continue;
				}
				else 
					PrintTsErr("write() returns error.");
				break;
			}
			
			circbuf.Clear(nWtn);
			
			ev.events |= EPOLLIN;
			
			// If buffer is empty, stop polling for writable
			if (nWtn == nstock) 
				ev.events &= ~EPOLLOUT;
		}
		
		if (ev.events != ev0)
		{
			err = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);		
			assert(!err);			
		}
	
		if (IsQuit())
			break;
	}

	printf("Echobuf water mark highest: %d bytes.\n", nWaterHigh);
		// [2018-01-07] Seen max 16380 on Raspberrry PI Zero W(RPI0).
		// Seems always multiple of 4095. (16380=4095*4)
	
	if(count_EAGAIN>0)
	{
		printf("write() encounters EAGAIN %d times (this is abnormal).\n", count_EAGAIN);
	}
	
	err = close(epollfd);
	if (err)
		PrintTsErr("close(epollfd) error.");
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("echosterm v%s, compiled on %s %s\n", VER_STR, __DATE__, __TIME__);
		printf("This program is an echo server on a terminal.\n");
		printf("During echo process, press Ctrl+C to quit.\n");
		printf("You should at least pass in a terminal device name to operate.\n");
		printf("For example:\n");
		printf("  echosterm /dev/ttyS0 -B 115200\n");
		printf("  echosterm /dev/ttyGS0\n");
		printf("  echosterm /dev/ttyACM0\n");
		printf("\n");
		printf("Options:\n");
		printf("  --bufsize 64000: Use a echo buffer size of 64000 bytes.\n");
		printf("  --epoll: Use epoll() to wait for readable/writable.\n");
		printf("  -B <baudrate> or --baudrate <baudrate>\n");
		exit(1);
	}

	parse_args(argc, argv);

	// This program is designed to have the following behavior:
	// User can press Ctrl+C from the terminal to break(stop) a select/poll/write
	// operation. Ctrl+C is not a way to force program quit.
	// To force program quit, user can press Ctrl+\ .

	if (!Settle_SIGINT())
		exit(1);

	int err = 0;
	int fd = open(g_szdev, O_RDWR|O_NOCTTY|O_NONBLOCK);
	
	if(fd==-1)
	{
		PrintErr(errno, "open('%s') fails.", g_szdev);
		return 1;
	}
	
	mm_printf("open('%s') success, nonblocking. Device-handle=0x%X\n", 
		g_szdev, (int)fd);
	
	if (!isatty(fd))
		ErrExit("This device is not a TTY device.");

	termios tio_orig = {};
	RawTermError_ret rte = termios_SetRawTerm(fd, g_baudrate, &tio_orig);
	if (rte == RTE_Success)
	{
		printf("%s terminal device init OK (baudrate=%d).\n", g_szdev, g_baudrate);
		if (g_use_epoll)
		{
			PrintTs("Waiting for echo client (epoll)...");
			start_EchoServer_epoll(fd, g_echobufsize);			
		}
		else
		{
			PrintTs("Waiting for echo client (simple)...");
			start_EchoServer_simple(fd, g_echobufsize);
		}
	}
	else 
	{
		ErrExit("Terminal device init error: %s", RTE2Name(rte));
	}
	
	PrintTs("close() device handle...");
	err = close(fd);
	if (!err)
		PrintTs("close() success.");
	else
		PrintTsErr("close() fail.");

//	report_SIGINT_count();
	
	return 0;
}

