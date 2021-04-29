﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
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

        void assert_main_thread() { Debug.Assert(get_thread_id() == s_mainthread_tid); }
        void assert_not_main_thread() { Debug.Assert(get_thread_id() != s_mainthread_tid); }

        #endregion

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
//          logtid($"Form1_Load. Main-thread-id={s_mainthread_tid}");
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            log("This program launches multiple asynchronus Tasks, trying to \"append\" to the same file. " +
                "We can observe whether it goes smoothly.");
        }

        async static Task<int> func_to_task(Func<Task<int>> func)
        {
            // There should be a system function for this...
            int ret = await func();
            return ret;
        }

        private Task _tsk;
        private CancellationTokenSource _cts;

        private async void btnStart_Click(object sender, EventArgs e)
        {
            if (_tsk != null)
            {
                Debug.Assert(_cts!=null);
                _cts.Cancel();

                btnStart.Enabled = false; // no need to click again.
                return;
            }

            try
            {
                btnStart.Text = "&Cancel";
                edtFilename.Enabled = false;
                edtLinesToWrite.Enabled = false;
                nudIotasks.Enabled = false;

                log("StartTest()");

                _cts = new CancellationTokenSource();
                _tsk = StartTest(_cts.Token);
                await _tsk;
            }
            catch (Exception exception)
            {
                log(exception.ToString());
            }
            finally
            {
                _cts = null;
                _tsk = null;

                btnStart.Text = "&Start";
                edtFilename.Enabled = true;
                edtLinesToWrite.Enabled = true;
                nudIotasks.Enabled = true;
            }
        }

        private async Task StartTest(CancellationToken ct)
        {
            string filename = edtFilename.Text;

            using( StreamWriter swriter = new StreamWriter(filename, false))
            {
                int ntasks = (int) nudIotasks.Value;

                // Prepare a bunch of ready-tasks.

                List<Task<int>> iotasks = new List<Task<int>>();
                for (int i = 0; i < ntasks; i++)
                {
                    iotasks.Add(Task.FromResult(i));
                }

                int next_line = 0;
                int total_lines = int.Parse(edtLinesToWrite.Text);
                int nfinished = 0;

                while (next_line < total_lines)
                {
                    // Wait for a done-slot, so we can go on queueing a WriteAsync task.

                    Task<int> tsk_done = await Task.WhenAny(iotasks);

                    int idx_done = await tsk_done;

                    next_line++;

                    iotasks[idx_done] = func_to_task(async () =>
                    {
                        // need these local capture before we await.
                        int local_idx_task = idx_done;
                        int local_next_line = next_line;

await Task.Delay(10);
                        await swriter.WriteAsync(
                            $"Line#{local_next_line,9}"+"\r\n".PadLeft(86, '.'));

                        nfinished++;

                        return local_idx_task;
                    });

                    lblQueued.Text = next_line.ToString();
                    lblFinished.Text = nfinished.ToString();
                }

                await Task.WhenAll(iotasks);
                lblFinished.Text = nfinished.ToString();

                log($"Task done: {Path.GetFullPath(filename)}");

            } // using file

        }
    }
}
