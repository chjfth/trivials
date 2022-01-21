using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// Code from [CSNUT7] CH14

namespace csTaskConti
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

        // Test Task-Continuation chain.

        static void TestContiChain1()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            log($"{funcname} Start.");

            Task<int> primeNumberTask = Task.Run(() =>
                Enumerable.Range(2, 30000).Count(n =>
                    Enumerable.Range(2, (int)Math.Sqrt(n) - 1).All(i => n % i > 0)));

            logtid($"primeNumberTask.GetHashCode()={primeNumberTask.GetHashCode()}");

            var tsk1a = primeNumberTask.ContinueWith(antecedent =>
            {
                int prev = antecedent.Result;
                int next = 10;
                logtid($"[1a]prev={prev} , next={next}"); // prev=3245
                return next;
            });

            var tsk1b = primeNumberTask.ContinueWith(delegate (Task<int> ant)
            {
                int prev = ant.Result;
                int next = 11;
                logtid($"[1b]prev={prev} , next={next}"); // prev=3245
                return 11;
            });

            var tsk2a = tsk1a.ContinueWith((Task<int> ant) =>
            {
                int result = ant.Result;
                logtid($"[2a]prev={result}"); // prev=10
                // No `return 12;` here, so this lambda function is an Action, not a Func .
            });

            /*
            // wrong code below: `tsk2a` and `ant` type not match!
            var tsk3a = tsk2a.ContinueWith(delegate (Task<int> ant) 
            {
                Console.WriteLine($"[3a]Result=...");
                return 10;
            });
            */

            tsk2a.Wait();
            logtid("tsk2a done.");

            tsk1b.Wait();
            logtid("tsk1b done.");

            log($"{funcname} End.");
        }


        static void Main(string[] args)
        {
            TestContiChain1();
/*
Running result: 
 * 1a and 1b always run from different threads.
 * 1a and 2a may be run from the same thread, or sometimes, from different threads.

[16:24:14.547          ] TestContiChain1() Start.
[16:24:14.551(+3ms)    ] <tid=main>  primeNumberTask.GetHashCode()=21083178
[16:24:14.563(+11ms)   ] <tid=8608>  [1b]prev=3245 , next=11
[16:24:14.563          ] <tid=10552> [1a]prev=3245 , next=10
[16:24:14.563          ] <tid=10552> [2a]prev=10
[16:24:14.563          ] <tid=main>  tsk2a done.
[16:24:14.563          ] <tid=main>  tsk1b done.
[16:24:14.563          ] TestContiChain1() End.
Press any key to continue . . . 

[16:29:47.706          ] TestContiChain1() Start.
[16:29:47.711(+4ms)    ] <tid=main>  primeNumberTask.GetHashCode()=21083178
[16:29:47.725(+13ms)   ] <tid=8584>  [1a]prev=3245 , next=10
[16:29:47.725          ] <tid=8576>  [1b]prev=3245 , next=11
[16:29:47.725          ] <tid=9616>  [2a]prev=10
[16:29:47.725          ] <tid=main>  tsk2a done.
[16:29:47.725          ] <tid=main>  tsk1b done.
[16:29:47.725          ] TestContiChain1() End.
Press any key to continue . . .

*/

            // ====================
        }
    }
}

// More to try: p581 TaskScheduler.UnobservedTaskException;
