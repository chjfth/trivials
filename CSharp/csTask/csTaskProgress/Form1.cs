using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Channels;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace prjSkeleton
{
    public partial class Form1 : Form
    {
        #region Logger

        static int get_thread_id() { return AppDomain.GetCurrentThreadId(); }
        static int s_mainthread_tid = get_thread_id();

        static readonly object _locker = new object();
        static DateTime s_last_DateTime = new DateTime(0);
        //
        static string text_elapse(int millisec)
        {
            if (millisec >= 1000)
            {
                double sec = (double)millisec / 1000;
                return $"(+{sec:g}s)";
            }
            else if (millisec > 0) return $"(+{millisec}ms)";
            else return "";
        }
        //
        private void log(string s)
        {
//          lock (_locker)
            {
                DateTime nowdt = DateTime.Now;
                int millisec_gap = (int)(nowdt - s_last_DateTime).TotalMilliseconds;
                int seconds_gap = millisec_gap / 1000;
                if (s_last_DateTime.Ticks > 0 && seconds_gap > 0)
                {
                    string text = "".PadLeft(Math.Min(10, seconds_gap), '.');
                    PrintLine(text);
                }

                string tsprefix = string.Format("[{0:D2}:{1:D2}:{2:D2}.{3:D3}{4,-10}] ",
                    nowdt.Hour, nowdt.Minute, nowdt.Second, nowdt.Millisecond, text_elapse(millisec_gap)
                );
                s_last_DateTime = nowdt;

                PrintLine($"{tsprefix}{s}");
            }
        }

        static string tid()
        {
            int tidnow = AppDomain.GetCurrentThreadId();
            if (tidnow == s_mainthread_tid)
                return "<tid=main>";
            else
                return $"<tid={tidnow}>";
        }

        void logtid(string s)
        {
            log(string.Format("{0,-11} {1}", tid(), s));
        }

        void PrintLine(string s)
        {
            Debug.WriteLine(s);
            textBox1.Invoke(new Action(() =>
            {
                textBox1.AppendText(s + Environment.NewLine);
            }));
        }

        void assert_main_thread() { Debug.Assert(get_thread_id()==s_mainthread_tid); }
        void assert_not_main_thread() { Debug.Assert(get_thread_id() != s_mainthread_tid); }

        #endregion

        public Form1()
        {
            InitializeComponent();
        }

        async void button1_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;

            await TaskGo();

            button1.Enabled = true;
        }

        async Task TaskGo()
        {
            // Two ways to attach progress callback.

            var progress = new Progress<int>(pct =>
            {
                assert_main_thread();

                logtid($"Working {pct}%");
            });

            progress.ProgressChanged += (Object sender, int pct) =>
            {
                Debug.Assert(sender is Progress<int>);
                Debug.Assert(pct is int);

                assert_main_thread();

                logtid($"See-ing {pct}%");
            };

            await MyTask(progress);

            logtid("Work Done.");
        }

        Task MyTask(IProgress<int> onProgressPercentChanged)
        {
            return Task.Run(() =>
            {
                const int max = 5;
                for (int i = 0; i < max; i++)
                {
                    logtid("Task thread reporting progress...");
                    assert_not_main_thread();

                    onProgressPercentChanged.Report((100*i) / max);
                    Thread.Sleep(1000);
                }

                onProgressPercentChanged.Report(100);
            });
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            logtid($"Form1_Load. Main-thread-id={s_mainthread_tid}");
        }

    }
}
