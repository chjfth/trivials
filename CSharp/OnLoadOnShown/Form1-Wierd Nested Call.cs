using System;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;

namespace OnLoadOnShown
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);

			string msg = "In override void OnLoad().";
			msgdbg(msg);
			msgbox(msg); // (BX) 
		}

		protected override void OnShown(EventArgs evt)
		{
			base.OnShown(evt);

			string msg = "In override void OnShown().";
			msgdbg(msg);
			msgbox(msg);
		}

		private static void msgdbg(string msg)
		{
			Debug.WriteLine("### " + msg);
		}
		private static void msgbox(string msg)
		{
			MessageBox.Show(msg, "Info", MessageBoxButtons.OK);
		}
	}
}

/* This is a bad program. OnLoad() will nest-call OnShown(), and OnShown() dialogbox
is displayed even before the OnLoad() one.
*/
