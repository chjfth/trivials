using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// Code from [CSNUT7] CH14

namespace csnutsTask1
{
    class Program
    {
        #region Logger
        static readonly object _locker = new object();
        static DateTime s_last_DateTime = new DateTime(0);
        //
        static string text_elapse(int millisec)
        {
            if (millisec >= 1000) {
                double sec = (double)millisec / 1000;
                return $"(+{sec:g}s)";
            }
            else if (millisec > 0) return $"(+{millisec}ms)";
            else return "";
        }
        //
        static void log(string s)
        {
            lock(_locker)
            {
                DateTime nowdt = DateTime.Now;
                int millisec_gap = (int) (nowdt - s_last_DateTime).TotalMilliseconds;
                int seconds_gap = millisec_gap / 1000;
                if (s_last_DateTime.Ticks > 0 && seconds_gap > 0)
                {
                    Console.Out.WriteLine("".PadLeft(Math.Min(10, seconds_gap), '.'));
                }

                string tsprefix = string.Format("[{0:D2}:{1:D2}:{2:D2}.{3:D3}{4}]",
                    nowdt.Hour, nowdt.Minute, nowdt.Second, nowdt.Millisecond, text_elapse(millisec_gap)
                );
                Console.Out.WriteLine($"{tsprefix}{s}");
                s_last_DateTime = nowdt;
            }
        }

        static string tid()
        {
            return $"<tid={AppDomain.GetCurrentThreadId()}>";
        }

        static void logtid(string s)
        {
            log($"{tid()} {s}");
        }
        #endregion

        private static int s_mainthread_tid = AppDomain.GetCurrentThreadId();

        static void AssertMainThread(bool b)
        {
            int tid = AppDomain.GetCurrentThreadId();
            if (b)
                Debug.Assert(s_mainthread_tid == tid);
            else
                Debug.Assert(s_mainthread_tid != tid);
        }

        ////////

        static void p579_CountPrimes()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name+"()";
            log($"{funcname} Start.");

            Task<int> primeNumberTask = Task.Run(() =>
                Enumerable.Range(2, 3000000).Count(n =>
                    Enumerable.Range(2, (int)Math.Sqrt(n) - 1).All(i => n % i > 0)
                )
            );

            // Fetching `primeNumberTask.Result` will block the caller until Task finish.
            Console.WriteLine("The answer is " + primeNumberTask.Result);

            log($"{funcname} End.");
        }

        static Task<int> p581_AwaiterConti()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Start.");

            Task<int> primeNumberTask = Task.Run(() =>
                Enumerable.Range(2, 3000000).Count(n =>
                    Enumerable.Range(2, (int)Math.Sqrt(n) - 1).All(i => n % i > 0)
                )
            );

            var awaiter = primeNumberTask.GetAwaiter();
            awaiter.OnCompleted(() =>
            {
                // This block of code is called Continuation(my abbr: Conti),
                // and its related Task is called antecedent task (my abbr: Antetask).
                logtid($"Conti executing...");

                AssertMainThread(false); // due to lack of Synchronization-context

                int result = awaiter.GetResult();
                Console.WriteLine("p581 answer is " + result); // Writes result
            });

            logtid($"{funcname} End.");

            return primeNumberTask;
        }

        static Task<int> GetPrimesCountAsync(int start, int count) // p591
        {
            return Task.Run(() =>
            {
                logtid($"Task-p591 executing...");

                int prime_count = ParallelEnumerable.Range(start, count).Count(n =>
                    Enumerable.Range(2, (int) Math.Sqrt(n) - 1).All(i => n % i > 0)
                    );

                return prime_count;
            });
        }

        static async void p591_Await()
        {
            string funcname = "p591_Await()";
            logtid($"{funcname} Start.");

            Console.Out.WriteLine("ChjNote: System.Reflection.MethodBase.GetCurrentMethod().Name=" +
                                  System.Reflection.MethodBase.GetCurrentMethod().Name);

            int result = await GetPrimesCountAsync(2, 1000000);

            logtid($"{funcname} Resume from await.");
            Console.WriteLine("p591 answer is " + result);

            logtid($"{funcname} End.");
        }

        static void Main(string[] args)
        {
            p579_CountPrimes();

            Task<int> task = null;
            task = p581_AwaiterConti();
            task.Wait();

            p591_Await();

            log("Main thread final Sleep then quit.");
            Thread.Sleep(3000);
        }
    }
}
