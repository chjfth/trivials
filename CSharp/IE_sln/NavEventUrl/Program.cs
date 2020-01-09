using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Security.Permissions;


[PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
[System.Runtime.InteropServices.ComVisibleAttribute(true)]
public class Form1 : Form
{
	private WebBrowser webBrowser1 = new WebBrowser();
	private Button button1 = new Button();
	int navs = 0;
	string urlnow = "http://localhost:88/";
	int tick_start, tick_prev; // millisecond count

	[STAThread]
	public static void Main()
	{
		Application.EnableVisualStyles();
		Application.Run(new Form1());
	}

	void PrintTs()
	{
		int now_tick = Environment.TickCount;

		if (now_tick - tick_prev >= 1000)
			Console.WriteLine("."); // add a blank line to ease eye viewing

		uint elapsed_msec = (uint)(now_tick - tick_start);
		Console.WriteLine($"[{elapsed_msec/1000}.{elapsed_msec%1000:D3}]");

		tick_prev = now_tick;
	}

	public Form1()
	{
		this.Text = "Navigating Event demo";
		this.Width = 540; // 540px width on a text-scale 100% monitor

		button1.Text = "Reset webBrowser1.DocumentText";
		button1.Dock = DockStyle.Top;
		button1.Click += new EventHandler(button1_Click);

		webBrowser1.Dock = DockStyle.Fill;

		webBrowser1.Navigating += 
			delegate (object sender, WebBrowserNavigatingEventArgs e)
			{
				navs++;
				PrintTs();
				Console.WriteLine($"({navs}) webBrowser1.Navigating");
                Console.WriteLine("  e.Url = " + e.Url);
				Console.WriteLine("  e.TargetFrameName = " + e.TargetFrameName);
			}; // C# 2.0 anonymous method

		webBrowser1.Navigated +=
			(object sender, WebBrowserNavigatedEventArgs e) =>
			{
				PrintTs();
				Console.WriteLine($"({navs}) webBrowser1.Navigated");
				Console.WriteLine("  e.Url = " + e.Url);
			}; // C# 3.0 lambda expression

		webBrowser1.DocumentCompleted +=
			(sender, e) => // e is WebBrowserDocumentCompletedEventArgs 
			{
				PrintTs();
				Console.WriteLine($"({navs}) webBrowser1.DocumentCompleted");
				Console.WriteLine("  e.Url = " + e.Url);
			}; // C# 3.0 lambda expression with implicit parameter type

		this.Controls.Add(webBrowser1);
		this.Controls.Add(button1);

		this.Load += new EventHandler(Form1_Load);

		tick_start = tick_prev = Environment.TickCount;
	}

	void ResetHtml()
	{
		string html = @"
<html>
<head>
<script>
	function UrlGo() 
	{ 
		var url = document.getElementById('url').value;
		window.external.SaveUrl(url);
		window.location.href = url;
	}
</script>
</head>
<body>
	<input type='text' style='width: 400px' id='url' value='[urlnow]'>
	<button onclick='UrlGo();'>Go</button>
</body>
</html>";
		html = html.Replace("[urlnow]", urlnow);
		webBrowser1.DocumentText = html;
	}

	private void Form1_Load(object sender, EventArgs e)
	{
		webBrowser1.AllowWebBrowserDrop = false;
		webBrowser1.IsWebBrowserContextMenuEnabled = true; // false will disable right-click menu
		webBrowser1.WebBrowserShortcutsEnabled = false;
		webBrowser1.ObjectForScripting = this;

		// Uncomment the following line when you are finished debugging.
		//webBrowser1.ScriptErrorsSuppressed = true;

		ResetHtml();
	}

	public void SaveUrl(String url)
	{
//		MessageBox.Show(url, "client code");
		urlnow = url;
	}

	private void button1_Click(object sender, EventArgs e)
	{
		ResetHtml();
	}

}
