﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Management;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace csAsyncInsight
{
    public partial class Form1 : Form
    {
        #region Logger
        private static int s_mainthread_tid = AppDomain.GetCurrentThreadId();

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

        #endregion

        #region UIcode

        public Form1()
        {
            InitializeComponent();

            ckbStickUIThread.Checked = true;
            ckbMainUISleep.Checked = true;

            this.MinimumSize = new Size(760, this.Size.Height);

            RunParamChanged(null, EventArgs.Empty);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void btnRun_Click(object sender, EventArgs e)
        {
            RunMain();
        }

        #endregion

        private static int s_UISleepMillis = 0;
        private static int s_TaskDelayMillis = 0;
        private static bool s_isStickUIThread = true;

        async Task<int> MyAsync(int is_await=0, int is_throw_before=0, int is_throw_after=0)
        {
            //string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            string funcname = $"MyAsync" + $"({is_await},{is_throw_before},{is_throw_after})";
            logtid($"{funcname} Start.");


            if (is_throw_before == 1)
            {
                logtid($"{funcname} throwing (before await)...");
                throw new ArgumentNullException("Null-Before-await");
            }

            if (is_await==1)
            {
                int delayms = s_TaskDelayMillis;

                Task tskdelay = Task.Delay(delayms);
                int hashcode = tskdelay.GetHashCode();

                /*
                tskdelay.ContinueWith((tskc) =>
                {   // This will be executed by worker thread!
                    logtid("tskdelay.ContinueWith() callback executing........");
                });
                */

                if (s_isStickUIThread)
                {
                    logtid($"{funcname} Task.Delay({delayms}) created and await it. tskdelay.GetHashCode()={hashcode}, tskdelay.Id={tskdelay.Id}");
                    await tskdelay;
                    logtid($"{funcname} Task.Delay()'s await done.");
                }
                else
                {
                    ConfiguredTaskAwaitable tsk_anyctx = tskdelay.ConfigureAwait(false);
                    logtid($"{funcname} Task.Delay({delayms}) created and await on its `tsk_anyctx`.");
                    await tsk_anyctx;
                    logtid($"{funcname} tsk_anyctx await done.");
                }
            }
            else
            {
                logtid($"{funcname} No executing Task.Delay(), no await.");
            }

            if (is_throw_after == 1)
            {
                logtid($"{funcname} throwing (after await)...");
                throw new ArgumentNullException("Null-After-await");
            }

            int ret = 8000 + is_await*100 + is_throw_before*10 + is_throw_after*1;
            logtid($"{funcname} End. Will return {ret}.");
            return ret;
        }

        void RunTask(int is_await = 0, int is_throw_before = 0, int is_throw_after = 0)
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + 
                              $"({is_await},{is_throw_before},{is_throw_after})";
            logtid($"{funcname} Start.");

            Task<int> tskout = MyAsync(is_await, is_throw_before, is_throw_after);
            ConfiguredTaskAwaitable<int> tskout_anyctx = tskout.ConfigureAwait(false);

            int ohash = tskout.GetHashCode();
            int ohash_anyctx = tskout_anyctx.GetHashCode();

            logtid($"{funcname} tskout.GetHashCode()={ohash} , tskout.Id={tskout.Id} ;" +
                   $" tskout_anyctx.GetHashCode()={ohash_anyctx}" ); 
                // Note: No such thing called ConfiguredTaskAwaitable.Id

            // Chj memo: I do not use `await tskout` but `tskout.GetAwaiter().OnCompleted()` here,
            // bcz I want to verify that :
            // "exceptions thrown from an async-method" are really "cached" in the associated tskout object,
            // and those exceptions can be retrieved from `awaiter.GetResult()`.

            if (s_isStickUIThread)
            {
                TaskAwaiter<int> awaiter = tskout.GetAwaiter();
                awaiter.OnCompleted(delegate()
                {
                    try
                    {
                        int rr = awaiter.GetResult();
                        logtid($"{funcname} awaiter.GetResult()={rr}");
                    }
                    catch (Exception e)
                    {
                        string info = $"{funcname} awaiter.GetResult() got exception.\r\n";
//                           info += $"e.Message=\r\n  {e.Message}";
                        logtid(info);
                    }
                });
            }
            else
            {
                ConfiguredTaskAwaitable<int>.ConfiguredTaskAwaiter awaiter = tskout_anyctx.GetAwaiter();
                awaiter.OnCompleted(delegate ()
                {
                    try
                    {
                        int rr = awaiter.GetResult();
                        logtid($"{funcname} awaiter.GetResult()={rr}");
                    }
                    catch (Exception e)
                    {
                        string info = $"{funcname} awaiter.GetResult() got exception.\r\n";
//                           info += $"e.Message=\r\n  {e.Message}";
                        logtid(info);
                    }
                });
            }

            logtid($"{funcname} End.");
        }

        private static int s_section_count = 0;

        void RunMain()
        {
            s_last_DateTime = DateTime.Now;
            if (this.ckbAppendText.Checked)
            {
                s_section_count++;
                PrintLine("");
                PrintLine($"---- [ {s_section_count} ] ----");
                PrintLine("");
            }
            else
            {
                s_section_count = 0;
                this.textBox1.Clear();

                // Note: After Clear(), We need Refresh() to force Clear UI drawing
                // before we return to Windows message loop.
                this.textBox1.Refresh(); 
            }

            RunParamChanged(null, EventArgs.Empty);

            RunTask(ckbEnableAwait.Checked ? 1 : 0, 
                ckbThrowBeforeAwait.Checked ? 1 : 0, 
                ckbThrowAfterAwait.Checked ? 1 : 0);

            if (s_UISleepMillis > 0)
            {
                int sleepms = s_UISleepMillis;
                logtid($"UI thread now sleep {sleepms} milliseconds...");
                Thread.Sleep(sleepms);
                logtid($"UI thread done sleep {sleepms} milliseconds.");
            }
        }

        private void btnClearText_Click(object sender, EventArgs e)
        {
            this.textBox1.Clear();
        }

        private void RunParamChanged(object sender, EventArgs e)
        {
            int is_await = ckbEnableAwait.Checked ? 1 : 0;
            int is_throw_before = ckbThrowBeforeAwait.Checked ? 1 : 0;
            int is_throw_after = ckbThrowAfterAwait.Checked ? 1 : 0;
            string param1 = $"({is_await},{is_throw_before},{is_throw_after})";

            this.lblRunParam.Text = param1;

            if (is_await==1)
            {
                lblTaskDelayMillis.Show();
                edtTaskDelayMillis.Show();
            }
            else
            {
                lblTaskDelayMillis.Hide();
                edtTaskDelayMillis.Hide();
            }
            //
            if (is_throw_before == 1)
            {
                ckbThrowAfterAwait.Hide();
            }
            else
            {
                ckbThrowAfterAwait.Show();
            }

            if (ckbMainUISleep.Checked)
            {
                s_UISleepMillis = int.Parse(edtUIThreadMillis.Text);
            }
            else
            {
                s_UISleepMillis = 0;
            }

            bool succ = int.TryParse(edtTaskDelayMillis.Text, out s_TaskDelayMillis);
            if (!succ || s_TaskDelayMillis<=0)
            {
                s_TaskDelayMillis = 1;
                edtTaskDelayMillis.Text = s_TaskDelayMillis.ToString();
            }

            s_isStickUIThread = ckbStickUIThread.Checked ? true : false;
        }
    }
}
