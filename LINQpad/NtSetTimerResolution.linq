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

void Main()
{
	dc = new DumpContainer(reso).Dump("Current NT timer resolution (unit is 100ns)");
	RefreshReso();

	var textBox = new TextBox(reso.cur.ToString(), width: "6em");
	var btnSet = new Button("NtSetTimerResolution", onClick: delegate
	{
		int userval = int.Parse(textBox.Text);
		int actual;
		int hresult = NtSetTimerResolution(userval, true, out actual);
		Debug.Assert(hresult == 0);
		$"actual value set: {userval} -> {actual}".Dump();
		RefreshReso();
	});
	new WrapPanel("1em", textBox, btnSet).Dump();
}

[DllImport("ntdll.dll", SetLastError = true)]
static extern int NtQueryTimerResolution(out int MaximumResolution, out int MinimumResolution, out int CurrentResolution);

[DllImport("ntdll.dll", SetLastError = true)]
static extern int NtSetTimerResolution(int DesiredResolution, bool SetResolution, out int CurrentResolution);
