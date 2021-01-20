﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Management;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

using ZjbLib;

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

        private void btnStart_Click(object sender, EventArgs e)
        {
            if (_tskHttp!=null)
            {
                Utils.WarnMsg("Task already running.");
                return;
            }

            StartHttp();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            if (_tskHttp == null)
            {
                Utils.WarnMsg("Task not running yet. Nothing to cancel.");
            }

            CancelHttp();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            logtid($"Form1_Load. Main-thread-id={s_mainthread_tid}");
        }

        ////

        private Task<string> _tskHttp;
        private CancellationTokenSource _cts;

        async void StartHttp()
        {
            // NOTE: For an `async void` function, we have to wrap all its code 
            // inside try-catch-block, so that exception does NOT leak into unknown world.
            try
            {
                //string url = "http://10.22.3.92:2017/";
                //string url = "http://10.22.3.84:8000/abc";
                string url = "http://10.22.244.44:4444/";

                AsyncHttp.HeaderDict headers = new AsyncHttp.HeaderDict()
                {
                    {
                        HttpRequestHeader.UserAgent,
                        "Mozilla/5.0 (Windows NT 10.0; Win64; x64; Trident/7.0; rv:11.0) like Gecko"
                    },
                };

                string postbody = "MyPostText";
                AsyncHttp ahttp = new AsyncHttp(url, headers, postbody);

                _cts = new CancellationTokenSource();

                //byte[] rsbytes = await ahttp.Start(_cts.Token, 15000);

                logtid("HTTP starts.");

                _tskHttp = ahttp.StartAsText(_cts.Token, 9900);
                string body = await _tskHttp;

                logtid(
                    $"HTTP success. Body bytes: {ahttp._respbody_bytes.Length}, body chars: {ahttp._respbody_text.Length}");
            }
            catch (Exception e)
            {
                if (e is WebException && 
                    ((WebException) e).Status == WebExceptionStatus.RequestCanceled
                    )
                {
                    logtid("HTTP request cancelled.");

                    string info = $"{e.Message}\r\n\r\n{e.StackTrace}";
                    MessageBox.Show(this, info, "Woo... Exception occurred.",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    logtid("HTTP fails with exception.");

                    string info = $"{e.Message}\r\n\r\n{e.StackTrace}";
                    MessageBox.Show(this, info, "Bad... Exception occurred.",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            finally
            {
                _tskHttp.Dispose();
                _tskHttp = null;
            } // try-block end

        } // async function end

        void CancelHttp()
        {
            try
            {
                _cts.Cancel();
            }
            catch (Exception e)
            {
                logtid("Unexpect! Exception in CancelHttp(). Program BUG!");
                Utils.ErrorMsg(e.Message);
            }
        }

    }
}
