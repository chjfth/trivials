﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
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
            ckbDoGC.Checked = true;

            long mem_used = GC.GetTotalMemory(true);
            logtid($"Program start. GC.GetTotalMemory()={mem_used}");
        }

        private void btnStartNoCancel_Click(object sender, EventArgs e)
        {
            TestOnce(false);
        }

        private void btnStartWithCancel_Click(object sender, EventArgs e)
        {
            TestOnce(true);
        }

        private int s_taskdelay_msec = 1000;
        private int s_mainthread_wait_msec = 2000;
        private int s_cycles = 123000;

        Task makeDelayTask()
        {
            Task tsk = Task.Delay(s_taskdelay_msec);
            return tsk;
        }

        Task makeDelayTask_and_Cancel()
        {
            CancellationTokenSource cts = new CancellationTokenSource();

            Task tsk = Task.Delay(s_taskdelay_msec, cts.Token);

            cts.Cancel();
            return tsk;
        }

        void CrazyTask(bool do_cancel)
        {
            for (int i = 0; i < s_cycles; i++)
            {
                if(!do_cancel)
                {
                    makeDelayTask();
                }
                else
                {
                    makeDelayTask_and_Cancel();
                }
            }
            return;
        }

        void TestOnce(bool do_cancel)
        {
            bool isGC = ckbDoGC.Checked;

            btnStartNoCancel.Enabled = false;
            btnStartWithCancel.Enabled = false;

            string mark = isGC ? "[GC]" : "";
            mark += do_cancel ? '-' : '*';

            long m1 = GC.GetTotalMemory(isGC);
            logtid($"{mark}MEM1: {m1}");

            CrazyTask(do_cancel);

            long m2 = GC.GetTotalMemory(isGC);
            logtid($"{mark}MEM2: {m2}");

            Thread.Sleep(s_mainthread_wait_msec); // 2000

            long m3 = GC.GetTotalMemory(isGC);
            logtid($"{mark}MEM3: {m3}");

            btnStartNoCancel.Enabled = true;
            btnStartWithCancel.Enabled = true;
        }

    }
}
