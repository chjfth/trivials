<Query Kind="Statements">
  <Namespace>System.Runtime.InteropServices</Namespace>
</Query>

[DllImport("Kernel32.dll", SetLastError = true)]
static extern bool QueryPerformanceCounter(ref ulong qpcvalue); // import this WinAPI
long GetTick_QPC()
{
	ulong qv = 0;
	bool succ = QueryPerformanceCounter(ref qv);
	return succ ? (long)qv : 0;
}

DateTime dt_now = DateTime.Now;
$"Current system time is: {dt_now.ToString("yyyy-MM-dd hh:mm:ss")}".Dump();

long ticks_qpc = GetTick_QPC();
$"Current QPC value: {ticks_qpc}".Dump();
DateTime dt_qpc = new DateTime(ticks_qpc);
(dt_qpc - new DateTime(0)).Dump("QPC as Timespan (system up time)");

DateTime dt_deduced_boottime = new DateTime(dt_now.Ticks - ticks_qpc);
dt_deduced_boottime.Dump("System boot time roughly at:");
// -- This should closely match that of `net statistics workstation`.
