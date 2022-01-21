using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
//using System.Windows.Controls;

namespace UIbyThread
{
    public partial class Form1 : Form
    {
        private int count = 0;

        public Form1()
        {
            InitializeComponent();

            // false will suppress debugger assert on wrong thread op
			// CheckForIllegalCrossThreadCalls = false; 

			string text =
@"Click a button to launch a thread.

※ For first button, the thread will call UI function directly; 
※ For second button, marshaling call is carried out. 

Run this program with VS debugger attached, the first button will cause 'Cross-thread operation not valid' exception. and the second will be fine.";

            this.textBox1.Text = text;
        }

        void UpdateUI(bool issafe)
        {
            count++;

            if (!issafe)
            {
                this.textBox2.Text = "Thread done: "+count; // call directly
            }
            else
            {
                Action action = delegate()
                    {
                        this.textBox2.Text = "Thread done: "+count;
                    };

				Console.WriteLine("MyBgThread Invoke() start...");

                this.Invoke(action); // call by marshaling

				Console.WriteLine("MyBgThread Invoke() end.");
			}
		}

        private void button1_Click(object sender, EventArgs e)
        {
            Thread thread = new Thread(() => UpdateUI(false));
            thread.Start();

            if(this.chkboxSleep.Checked)
                Thread.Sleep(2000);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Thread thread = new Thread(() => UpdateUI(true));
			thread.Name = "MyBgThread";
            thread.Start();

            if (this.chkboxSleep.Checked)
                Thread.Sleep(2000);
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
