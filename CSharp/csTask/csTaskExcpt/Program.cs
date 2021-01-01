using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// Code from [CSNUT7] CH14

namespace csTaskExcpt
{
    class Program
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
        static void log(string s)
        {
            lock (_locker)
            {
                DateTime nowdt = DateTime.Now;
                int millisec_gap = (int)(nowdt - s_last_DateTime).TotalMilliseconds;
                int seconds_gap = millisec_gap / 1000;
                if (s_last_DateTime.Ticks > 0 && seconds_gap > 0)
                {
                    Console.Out.WriteLine("".PadLeft(Math.Min(10, seconds_gap), '.'));
                }

                string tsprefix = string.Format("[{0:D2}:{1:D2}:{2:D2}.{3:D3}{4,-10}] ",
                    nowdt.Hour, nowdt.Minute, nowdt.Second, nowdt.Millisecond, text_elapse(millisec_gap)
                );
                Console.Out.WriteLine($"{tsprefix}{s}");
                s_last_DateTime = nowdt;
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

        static void logtid(string s)
        {
            log(string.Format("{0,-11} {1}", tid(), s));
        }
        #endregion

        static void p580_TaskThrowExcpt()
        {
            // Start a Task that throws a NullReferenceException:

            logtid("Now start a Task.");

            Task task = Task.Run(() =>
            {
                logtid("Task executing, throwing an Exception.");
                throw null;
            });

            logtid("Task launched, main-thread sleep for 500ms, we'll see.");
            Thread.Sleep(500);

            try
            {
                logtid("Calling task.Wait()");
                task.Wait();
            }
            catch (AggregateException aex)
            {
                logtid("Got Exception.");
                if (aex.InnerException is NullReferenceException)
                    Console.WriteLine("Null!");
                else
                    throw;
            }
        }

        static void Main(string[] args)
        {
            p580_TaskThrowExcpt();
        }
    }
}
