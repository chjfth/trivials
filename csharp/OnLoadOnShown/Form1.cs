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

			this.Load += delegate
			{
				string msg = "In .Load event.";
				msgdbg(msg);
				msgbox(msg);
			};

			this.Shown += delegate
			{
				string msg = "In .Shown event.";
				msgdbg(msg);
				msgbox(msg);
			};
		}

		protected override void OnLoad(EventArgs e)
		{
			string msg = "In override void OnLoad().";
			msgdbg(msg); 
//			msgbox(msg);

			base.OnLoad(e);
		}

		protected override void OnShown(EventArgs evt)
		{
			string msg = "In override void OnShown().";
			msgdbg(msg);
//			msgbox(msg);

			base.OnShown(evt);
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
