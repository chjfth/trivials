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
				Debug.WriteLine(msg);
				msgbox(msg);
			};

			this.Shown += delegate
			{
				string msg = "In .Shown event.";
				Debug.WriteLine(msg);
				msgbox(msg);
			};
		}

		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);

			string msg = "In override void OnLoad().";
			Debug.WriteLine(msg);
//			msgbox(msg);
		}

		protected override void OnShown(EventArgs evt)
		{
			base.OnShown(evt);

			string msg = "In override void OnShown().";
			Debug.WriteLine(msg);
//			msgbox(msg);
		}

		private static void msgbox(string msg)
		{
			MessageBox.Show(msg, "Info", MessageBoxButtons.OK);
		}
	}
}
