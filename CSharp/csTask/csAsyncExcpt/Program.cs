using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// Chj: Wrap an `await ...` inside a try block, and sees that we can catch exception
// from the async task.
// But be aware that the thread executing catch{...} may not be the thread that started
// the await statement.

namespace csAsyncExcpt
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

        async static Task TaskThrow_after(int millisec)
        {
            Task tsk = Task.Run(() =>
            {
                logtid($"Task executing, throwing an Exception after {millisec} millisec.");
                Thread.Sleep(2000);
                throw null;
            });
            await tsk;
        }

        async static Task Do_AsyncExcpt1()
        {
//          string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            string funcname = "Do_AsyncExcpt1()";
            logtid($"{funcname} Start.");

            try
            {
                await TaskThrow_after(2000);

                // Since TaskThrow_after() will throw exception,
                // remaining code of this code-block will not get executed.

                Console.Out.WriteLine("Will not see this!");
                Debug.Assert(false);
            }
            catch (Exception e)
            {
                logtid($"OK. We caught the Exception.\ne.Message=\n  {e.Message}");
//              Console.WriteLine(e);
                
                logtid($"OK. Exception settled, await goes on.");
            }

            logtid($"{funcname} End.");
        }

        static void Test_AsyncExcpt1()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Start.");

            Task tsk = Do_AsyncExcpt1();

            logtid($"{funcname} Waiting task done...");
            tsk.Wait();

            logtid($"{funcname} End.");
        }

        static void Main(string[] args)
        {
            Test_AsyncExcpt1();
        }
    }
}
