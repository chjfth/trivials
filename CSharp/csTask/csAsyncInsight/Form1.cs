using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace csAsyncInsight
{
    public partial class Form1 : Form
    {
        private static object locker = new object();
        public Form1()
        {
            InitializeComponent();
        }

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
            lock (_locker)
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
            textBox1.BeginInvoke(new Action(() =>
            {
                textBox1.AppendText(s + Environment.NewLine);
            }));
        }

        #endregion

        #region UIcode
        private void Log(string s)
        {
            textBox1.BeginInvoke(new Action(() =>
            {
                textBox1.AppendText(s + Environment.NewLine);
            }));
        }

        private void button1_Click(object sender, EventArgs e)
        {
            RunMain();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            RunMain();
        }
        #endregion


        async Task<int> MyAsync(int is_await=0, int is_throw_before=0, int is_throw_after=0)
        {
            //string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            string funcname = "MyAsync";
            logtid($"{funcname} Start. (is_await={is_await}, is_throw_before={is_throw_before}, is_throw_after={is_throw_after})");


            if (is_throw_before==1)
                throw new ArgumentNullException("Null-Before-await");

//            Thread.Sleep(500);

            if (is_await==1)
            {
                Task tskdelay = Task.Delay(1000);
                int hashcode = tskdelay.GetHashCode();

                logtid($"Task.Delay(1000) created. tskdelay.GetHashCode()={hashcode}, tskdelay.Id={tskdelay.Id}");

                await tskdelay;
            }

            if (is_throw_after==1)
                throw new ArgumentNullException("Null-After-await");

//            Thread.Sleep(500);

            int ret = 8000 + is_await*100 + is_throw_before*10 + is_throw_after*1;
            logtid($"{funcname} End. Will return {ret}.");
            return ret;
        }

        void See_TaskId(int is_await = 0, int is_throw_before = 0, int is_throw_after = 0)
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Start.");

            Task<int> tskout = MyAsync(is_await, is_throw_before, is_throw_after);

            int ohash = tskout.GetHashCode();
            logtid($"tskout.GetHashCode()={ohash} , tskout.Id={tskout.Id}");

            var awaiter = tskout.GetAwaiter();
            awaiter.OnCompleted(delegate ()
            {
                try
                {
                    int rr = awaiter.GetResult();
                    logtid("awaiter.GetResult()=" + rr);
                }
                catch (Exception e)
                {
                    string info = $"awaiter.GetResult() got exception.\r\n" +
                                  $"e.Message=\r\n" +
                                  $"  {e.Message}";
                    logtid(info);
                }
            });

            logtid($"{funcname} End.");
        }


        void RunMain()
        {
            See_TaskId(0, 0, 0);
            PrintLine("----");

            See_TaskId(1, 0, 0);
            PrintLine("----");

            See_TaskId(1, 1, 0);
            PrintLine("----");

        }
    }
}
