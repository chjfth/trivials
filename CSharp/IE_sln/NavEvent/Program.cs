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
	int count = 0;

	[STAThread]
	public static void Main()
	{
		Application.EnableVisualStyles();
		Application.Run(new Form1());
	}

	public Form1()
	{
		this.Text = "Navigating Event demo";
		this.Width = 360; // 360px width on a text-scale 100% monitor

		button1.Text = "Set a new webBrowser1.DocumentText";
		button1.Dock = DockStyle.Top;
		button1.Click += new EventHandler(button1_Click);

		webBrowser1.Dock = DockStyle.Fill;

		webBrowser1.Navigating += 
			delegate (object sender, WebBrowserNavigatingEventArgs e)
			{
				Console.WriteLine($"({count}) webBrowser1.Navigating");
                Console.WriteLine("  e.Url = " + e.Url);
				Console.WriteLine("  e.TargetFrameName = " + e.TargetFrameName);
			}; // C# 2.0 anonymous method

		webBrowser1.Navigated +=
			(object sender, WebBrowserNavigatedEventArgs e) =>
			{
				Console.WriteLine($"({count}) webBrowser1.Navigated");
				Console.WriteLine("  e.Url = " + e.Url);
			}; // C# 3.0 lambda expression

		webBrowser1.DocumentCompleted +=
			(sender, e) => // e is WebBrowserDocumentCompletedEventArgs 
			{
				Console.WriteLine($"({count}) webBrowser1.DocumentCompleted");
				Console.WriteLine("  e.Url = " + e.Url);
			}; // C# 3.0 lambda expression with implicit parameter type

		this.Controls.Add(webBrowser1);
		this.Controls.Add(button1);

		this.Load += new EventHandler(Form1_Load);
	}

	void SetNewHtml()
	{
		webBrowser1.DocumentText =
			"<html><head><script>" +
			"function jstest(message) { alert(message); return 'JS Done.'; }" +
			"</script></head><body><button " +
			"onclick=\"window.external.csTest('called from script code')\">" +
			$"({count}) call client code from script code</button>" +
			"</body></html>";
	}

	private void Form1_Load(object sender, EventArgs e)
	{
		webBrowser1.AllowWebBrowserDrop = false;
		webBrowser1.IsWebBrowserContextMenuEnabled = false;
		webBrowser1.WebBrowserShortcutsEnabled = false;
		webBrowser1.ObjectForScripting = this;

		// Uncomment the following line when you are finished debugging.
		//webBrowser1.ScriptErrorsSuppressed = true;

		SetNewHtml();
	}

	public void csTest(String message)
	{
		MessageBox.Show(message, "client code");
	}

	private void button1_Click(object sender, EventArgs e)
	{
		count++;
		SetNewHtml();
	}

}
