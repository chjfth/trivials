<Query Kind="Statements" />


static long UintTickCount()
{
	int tc = Environment.TickCount;
	if (tc>=0)
		return tc;
	else
		return ((long)1<<32) + tc;
}

// Create Answer box

var listTicks = new List<string>();
var listSummary = new List<string>();
var yourResult = new DumpContainer(listTicks);
yourResult.AppendContent(listSummary);

void ProbeResolution(Func<long> gettick)
{
	listTicks.Clear();
	listSummary.Clear();
	
	long millisec_start = UintTickCount(); // use this as wall time elapse
	
	long starttick = gettick(); 
	long prevtick = starttick;

	for (int i = 0; i < 1000; i++)
	{
		long nowtick;
		while (true)
		{
			// Stay here until the reported tick move forward.
			nowtick = gettick();
			if (nowtick != prevtick)
				break;
		}

		listTicks.Add($"[{i}] {nowtick} (+{nowtick - prevtick})");
		yourResult.Refresh();

		prevtick = nowtick;
	}
	
	// Let's deduce how many ticks are in a second.
	
	long endtick = gettick();
	long total_ticks = endtick -starttick;

	long millisec_end = UintTickCount(); // use this as wall time elapse
	uint millisec_elapsed = (uint)(millisec_end - millisec_start);
	// -- convert to uint so that we can cope with 32-bit TickCount wrap around.

	long ticks_per_second = 1000 * total_ticks / millisec_elapsed;
	listSummary.Add($"Ticks per second: {ticks_per_second}");
	yourResult.Refresh();
}

// Create Question box

var ops = new Dictionary<string, Func<long>>()
{
	["DateTime.Now.Ticks"] = () => DateTime.Now.Ticks,
	["Environment.TickCount"] = UintTickCount,
};

var yourChoice = new DumpContainer();

string[] menuitems = ops.Keys.ToArray();
var listBox = new LINQPad.Controls.SelectBox(menuitems, 0, 
	lb => ProbeResolution(ops[(string)lb.SelectedOption])
	);

// Show Question and Answer box

"Select a Ticking method to go:".Dump();

listBox.Dump();
yourResult.Dump();

ProbeResolution(ops[(string)listBox.SelectedOption]);

