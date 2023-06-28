#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int g_delay_sec = 0;
// -- Can be positive or negative.
// If positive, myConsoleHandler will return TRUE.
// If negative, myConsoleHandler will return FALSE.

char* now_timestr(char buf[], int bufchars, bool ymd = false)
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	buf[0] = '['; buf[1] = '\0'; buf[bufchars-1] = '\0';
	if (ymd) {
		_snprintf_s(buf, bufchars-1, _TRUNCATE, "%s%04d-%02d-%02d ", buf,
			st.wYear, st.wMonth, st.wDay);
	}
	_snprintf_s(buf, bufchars-1, _TRUNCATE, "%s%02d:%02d:%02d]", buf,
		st.wHour, st.wMinute, st.wSecond);
	return buf;
}


BOOL WINAPI myConsoleHandler(DWORD dwCtrlType)
{
	const char* evtext = "Unknown event";
	if (dwCtrlType == CTRL_C_EVENT)
		evtext = "CTRL_C_EVENT";
	else if (dwCtrlType == CTRL_BREAK_EVENT)
		evtext = "CTRL_BREAK_EVENT";
	else if (dwCtrlType == CTRL_CLOSE_EVENT)
		evtext = "CTRL_CLOSE_EVENT";
	else if (dwCtrlType == CTRL_LOGOFF_EVENT)
		evtext = "CTRL_LOGOFF_EVENT";
	else if (dwCtrlType == CTRL_SHUTDOWN_EVENT)
		evtext = "CTRL_SHUTDOWN_EVENT";

	char textbuf[80] = {}, timebuf[40] = {};
	_snprintf_s(textbuf, _TRUNCATE, "%s Got event %s\n",
		now_timestr(timebuf, ARRAYSIZE(timebuf), true),
		evtext);

	const char* szfilename = "CuiNoBreak.log";

	printf(">> %s : %s", szfilename, textbuf);

	FILE* fp = nullptr;
	errno_t err = fopen_s(&fp, szfilename, "a");
	if (err == 0)
	{
		fwrite(textbuf, strlen(textbuf), 1, fp);
		fclose(fp);
	}
	else
	{
		printf(">> !! Cannot create file %s\n", szfilename);
	}

	int delay_sec = g_delay_sec > 0 ? g_delay_sec : -g_delay_sec;
	bool rettrue = g_delay_sec > 0 ? true : false;

	if (delay_sec > 0)
	{
		printf(">> Delay %d seconds before return %s\n",
			delay_sec,
			rettrue ? "TRUE" : "FALSE");
		Sleep(delay_sec * 1000);
		printf(">> Delay done.\n");
	}

	return rettrue ? TRUE : FALSE;
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		printf("Installing Ctrl+C handler...\n");

		BOOL succ = SetConsoleCtrlHandler(myConsoleHandler, TRUE);
		if (succ)
			printf("Installing Ctrl+C handler success.\n");
		else
			printf("Installing Ctrl+C handler fail! WinErr=%d\n", GetLastError());

		g_delay_sec = atoi(argv[1]);
	}

	int cycles = 10;
	for (int i = 0; i < cycles; i++)
	{
		printf("Sleep cycles: %d\n", i + 1);
		Sleep(1000);
	}

	printf("Done.\n");
}
