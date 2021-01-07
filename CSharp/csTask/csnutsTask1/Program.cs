using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Nito.AsyncEx;

// Code from [CSNUT7] CH14

namespace csnutsTask1
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
                    PrintLine("".PadLeft(Math.Min(10, seconds_gap), '.'));
                }

                string tsprefix = string.Format("[{0:D2}:{1:D2}:{2:D2}.{3:D3}{4,-10}] ",
                    nowdt.Hour, nowdt.Minute, nowdt.Second, nowdt.Millisecond, text_elapse(millisec_gap)
                );
                PrintLine($"{tsprefix}{s}");
                s_last_DateTime = nowdt;
            }
        }

        static void PrintLine(string s)
        {
            Console.WriteLine(s);
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


        static void AssertMainThread(bool b)
        {
            int tid = AppDomain.GetCurrentThreadId();
            if (b)
                Debug.Assert(s_mainthread_tid == tid);
            else
                Debug.Assert(s_mainthread_tid != tid);
        }

        ////////

        static void p579_CountPrimes_SingleCore()
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

        static void p588_CountPrimes_MultiCore()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            log($"{funcname} Start.");

            Task<int> primeNumberTask = Task.Run(() =>
                ParallelEnumerable.Range(2, 3000000).Count(n =>
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

                // Add a sleep here so that we can see whether main-thread's task.Wait() covers this.
                // This result is: No.
                Thread.Sleep(222); 

                int result = awaiter.GetResult();
                logtid("p581 answer is " + result); // Writes result
                Debug.Assert( result == primeNumberTask.Result );
            });

            logtid($"{funcname} End.");

            return primeNumberTask;
        }

        static Task<int> GetPrimesCountAsync(int start, int count) // p588 & p591
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

        static void p588_TrickyUseOfTask()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Start. Just experiment, with tricky output.");

            int complete_count = 0;

            for (int i = 0; i < 10; i++)
            {
                var awaiter = GetPrimesCountAsync(i * 1000000 + 2, 1000000).GetAwaiter();
                awaiter.OnCompleted(delegate ()
                    {
                        complete_count++;
                        logtid(awaiter.GetResult() + $" primes between... (i={i})");
                    }
                );
            }

            logtid($"Now, we need to wait that bunch of tasks done...");

            while (complete_count < 10) Thread.Sleep(100);

            logtid($"{funcname} Done tricky experiment.");
        }

        static async Task<int> p591_CountPrimes_AsyncWay()
        {
            string funcname = "p591_CountPrimes_AsyncWay()";
            logtid($"{funcname} Start.");

            Console.Out.WriteLine("ChjNote: System.Reflection.MethodBase.GetCurrentMethod().Name=" +
                                  System.Reflection.MethodBase.GetCurrentMethod().Name);

            int result = await GetPrimesCountAsync(2, 1000000);

            logtid($"{funcname} ~~~ Resume from await.");
            Console.WriteLine("p591 answer is " + result);

            logtid($"{funcname} End.");

            return result;
        }

        ////

        static void Main(string[] args)
        {
            var syncctx = SynchronizationContext.Current;

            p588_CountPrimes_MultiCore();

            ////

            Console.Out.WriteLine("");

            Task<int> task = null;
            task = p581_AwaiterConti();
            task.Wait();
            logtid($"task.Wait() success, task.Status={task.Status}, task.Result={task.Result}");

            Thread.Sleep(222+50);

            ////

            Console.Out.WriteLine("");
            p588_TrickyUseOfTask();

            ////

            Console.Out.WriteLine("");

            task = p591_CountPrimes_AsyncWay();
            //
            logtid("Main-thread calls task.Wait() .");
            task.Wait();
            logtid("Main-thread done task.Wait() .");

            ////

            try
            {
                // Try recipe 2.9 from *Concurrency in C# Cookbook* .

                log("");
                log("==== Now we will run p591_CountPrimes_AsyncWay() inside an AsyncContext. ====");
                log("============  This time, \"Resume from same thread\" is expected.   ===========");
                log("");
                int ret = AsyncContext.Run(() => p591_CountPrimes_AsyncWay());
                logtid($"Return value from async p591_CountPrimes_AsyncWay() : {ret}");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                throw;
            }

            ////

            /*
            try
            {
                log("");
                int ret = AsyncContext.Run(() => p581_AwaiterConti());
                logtid($"Return value from async p581_AwaiterConti() : {ret}");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                throw;
            }
            */

            ////

            log("==== Main thread final Sleep then quit. ====");
            Thread.Sleep(500);
        }
    }
}
