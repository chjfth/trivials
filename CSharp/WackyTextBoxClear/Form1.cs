using System;
using System.Data;
using System.Drawing;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace WackyTextBoxClear
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            btnFill.Text = "&Fill";
            btnClearNoRefresh.Text = "&Clear, no Refresh !";
            btnClearAndRefresh.Text = "&Clear, and Refresh";

            lblHint.Text = 
                "Click Fill to fill some old text into the Editbox, " +
                "then click one of the Clear buttons. We'll see different " +
                "effect with and without textBox.Refresh() call.";
        }

        private void btnFill_Click(object sender, EventArgs e)
        {
            textBox1.Text = "Text Line 1.\r\n" +
                            "Text Line 2.\r\n" +
                            "Text Line 3.\r\n";
        }

        private void btnClearNoRefresh_Click(object sender, EventArgs e)
        {
            DoClear(false);
        }

        private void btnClearAndRefersh_Click(object sender, EventArgs e)
        {
            DoClear(true);
        }

        private void DoClear(bool is_refresh)
        {
            textBox1.Clear();

            if(is_refresh)
                textBox1.Refresh();

            textBox1.AppendText("Sleep for 1 second...");

            Thread.Sleep(1000);
        }

    }
}
