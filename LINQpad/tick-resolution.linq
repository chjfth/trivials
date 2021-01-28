<Query Kind="Statements">
  <Namespace>LINQPad.Controls</Namespace>
</Query>


DateTime dt_origin = DateTime.UtcNow;
long WalltimeMillisec()
{
	return (long)(DateTime.UtcNow - dt_origin).TotalMilliseconds;
}

static long UintTickCount() // wrapper for Environment.TickCount
{
	int tc = Environment.TickCount;
	if (tc>=0)
		return tc;
	else
		return ((long)1<<32) + tc;
}

// Create Answer box

var listTicks = new List<string>();
var lblSummary = new Label("...");
var yourResult = new DumpContainer(lblSummary);
yourResult.AppendContent(listTicks);

void ProbeResolution(Func<long> gettick)
{
	listTicks.Clear();
	lblSummary.Text = "Probing...";

	long millisec_start = WalltimeMillisec(); // use this as wall time elapse

	long starttick = gettick();
	long prevtick = starttick;

	// We'll run at least two seconds.
	int inner_cycles = 100;
	for(int outer=0; outer<50; outer++)
	{
		for (int i = 0; i < 100; i++)
		{
			long nowtick;
			while (true)
			{
				// Stay here until the reported tick move forward.
				nowtick = gettick();
				if (nowtick != prevtick)
					break;
			}
	
			int idx_total = outer*inner_cycles + i;
			
			listTicks.Add($"[{idx_total}] {nowtick} (+{nowtick - prevtick})");
			yourResult.Refresh();
	
			prevtick = nowtick;
		}
		
		if(WalltimeMillisec()-millisec_start > 2000)
			break;
	}

	// Let's deduce how many ticks are in a second.
	
	long endtick = gettick();
	long total_ticks = endtick -starttick;

	long millisec_end = WalltimeMillisec(); 
	uint millisec_elapsed = (uint)(millisec_end - millisec_start);
	// -- convert to uint so that we can cope with 32-bit TickCount wrap around.

	long ticks_per_second = 1000 * total_ticks / millisec_elapsed;
	lblSummary.Text = $"Total run {millisec_elapsed} millisec , Ticks per second: {ticks_per_second}";
	yourResult.Refresh();
}

var swatch = new Stopwatch();
swatch.Start();

// Create Question box

var ops = new Dictionary<string, Func<long>>()
{
	["Environment.TickCount"] = UintTickCount,
	["DateTime.Now.Ticks"] = () => DateTime.Now.Ticks,
	["Stopwatch.ElapsedTicks"] = () => swatch.ElapsedTicks,
	["Stopwatch.ElapsedMilliseconds"] = () => swatch.ElapsedMilliseconds ,
};

//var yourChoice = new DumpContainer();

string[] menuitems = ops.Keys.ToArray();
var listBox = new LINQPad.Controls.SelectBox(menuitems, 0);
var btnRunAgain = new Button("Run again");
btnRunAgain.Click += delegate { RunAgain(); };
listBox.SelectionChanged += delegate { RunAgain(); };

// Show Question and Answer box

void RunAgain()
{
	btnRunAgain.Enabled = false;
	listBox.Enabled = false;
	
	ProbeResolution(ops[(string)listBox.SelectedOption]);
	
	btnRunAgain.Enabled = true;
	listBox.Enabled = true;
}

"Select a Ticking method to go:".Dump();

new WrapPanel("2em", listBox, btnRunAgain).Dump();
//
yourResult.Dump();
RunAgain();
