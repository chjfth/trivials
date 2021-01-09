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

// Key point:
// Task progress reporting and the ability to cancel our own Task.

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

        private CancellationTokenSource cts;

        public Form1()
        {
            InitializeComponent();

            this.Text = "csTaskProgress";
            this.edtRunSeconds.Text = "5";

            Button_MarkRunning(false);
        }

        void Button_MarkRunning(bool run)
        {
            if (run)
            {
                btnRun.Enabled = false;
                btnCancel.Enabled = true;
                btnCancel.Focus();
            }
            else
            {
                btnRun.Enabled = true;
                btnCancel.Enabled = false;
                btnRun.Focus();
            }
        }

        async void btnRun_Click(object sender, EventArgs e)
        {
            s_last_DateTime = DateTime.Now;
            textBox1.Clear();
            textBox1.Refresh(); // to ensure redraw in case we stay in main UI for too long

            logtid("Run button clicked.");
            Button_MarkRunning(true);

            cts = new CancellationTokenSource();
            using (cts)
            {
                await TaskGo(cts.Token);
            }
            cts = null;

            Button_MarkRunning(false);
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            logtid("Cancel button clicked.");
            cts.Cancel();
        }

        async Task TaskGo(CancellationToken ct)
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

            try
            {
                int run_seconds = int.Parse(edtRunSeconds.Text);
                await MyTask(run_seconds, progress, ct);
                logtid("Work Done.");
            }
            catch (OperationCanceledException e)
            {
                logtid("Work cancelled by user.");
            }
            catch (Exception e)
            {
                string info = $"Got exception:\r\n" +
                              $"  {e.Message}\r\n" +
                              $"Work failed!";
                logtid(info);
            }
        }

        Task MyTask(int run_seconds, IProgress<int> onProgressPercentChanged, CancellationToken ct)
        {
            return Task.Run(() =>
            {
                for (int i = 0; i < run_seconds; i++)
                {
                    logtid("Task thread reporting progress...");
                    assert_not_main_thread();

                    onProgressPercentChanged.Report((100*i) / run_seconds);

                    bool is_canceled = ct.WaitHandle.WaitOne(1000);
                    if (is_canceled)
                    {
                        ct.ThrowIfCancellationRequested();
                    }
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
