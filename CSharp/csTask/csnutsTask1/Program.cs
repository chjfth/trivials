using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// Code from [CSNUT7] CH14

namespace csnutsTask1
{
    class Program
    {
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
            DateTime nowdt = DateTime.Now;
            int millisec_gap = (int) (nowdt - s_last_DateTime).TotalMilliseconds;
            int seconds_gap = millisec_gap/1000;
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

        static string tid()
        {
            return $"<tid={AppDomain.GetCurrentThreadId()}>";
        }

        static void logtid(string s)
        {
            log($"{tid()} {s}");
        }

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
                // 不妨把这个回调函数体叫做 conti, 它的前情任务, 书中称之 antecedent task (antetask).
                logtid($"Task Conti executing...");

                int result = awaiter.GetResult();
                Console.WriteLine("p581 answer is " + result); // Writes result
            });

            logtid($"{funcname} End.");

            return primeNumberTask;
        }

        static void Main(string[] args)
        {
            p579_CountPrimes();

            Task<int> task = null;
            task = p581_AwaiterConti();

            task.Wait();

            log("Main thread final Sleep then quit.");
            Thread.Sleep(500);
        }
    }
}
