<Query Kind="Program">
  <Namespace>System.Runtime.InteropServices</Namespace>
  <Namespace>LINQPad.Controls</Namespace>
</Query>

class Reso {
	public int max;
	public int min;
	public int cur;
}

Reso reso = new Reso();
DumpContainer dc;

int RefreshReso()
{
	int hresult = NtQueryTimerResolution(out reso.max, out reso.min, out reso.cur);
	Debug.Assert(hresult==0);
	dc.Refresh();
	return reso.cur;
}

string ResoHint(int val)
{
	if (val < 10)
		return $"{val*100}ns";
	else if (val < 10 * 1000)
		return $"{val / 10}μs {val % 10 *100}ns";
	else
		return $"{val/10000}ms {val/10%1000}μs {val % 10 *100}ns";
}

void Main()
{
	dc = new DumpContainer(reso).Dump("Current NT timer resolution (unit is 100ns)");
	RefreshReso();

	var textBox = new TextBox(reso.cur.ToString(), width: "6em"); 
	var lblHint = new Label(ResoHint(reso.cur));
	textBox.TextInput += (sender, args) => {
		if(textBox.Text=="")
			return;
		lblHint.Text = ResoHint(int.Parse(textBox.Text));
	};
	var btnSet = new Button("NtSetTimerResolution", onClick: delegate
	{
		int userval = int.Parse(textBox.Text);
		int actual;
		int hresult = NtSetTimerResolution(userval, true, out actual);
		Debug.Assert(hresult == 0);
		$"actual value set: {userval} -> {actual}".Dump();
		RefreshReso();
		textBox.Text = reso.cur.ToString();
		ResoHint(reso.cur);
	});
	new WrapPanel("1em", textBox, btnSet).Dump();
	lblHint.Dump();
	"".Dump(); // add an empty line
}

[DllImport("ntdll.dll", SetLastError = true)]
static extern int NtQueryTimerResolution(out int MaximumResolution, out int MinimumResolution, out int CurrentResolution);

[DllImport("ntdll.dll", SetLastError = true)]
static extern int NtSetTimerResolution(int DesiredResolution, bool SetResolution, out int CurrentResolution);
