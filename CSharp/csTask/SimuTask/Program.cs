using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// Code from [CSNUT7] CH14
// Demo the use of class TaskCompletionSource.

namespace SimuTask
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

        static void p583_TurnThreadIntoTask()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Starting a thread, thread+TaskCompletionSource=Task.");

            var tcs = new TaskCompletionSource<int>();

            new Thread(() =>
                {
                    logtid("Thread is executing...");
                    Thread.Sleep(1000); tcs.SetResult(42);
                }
            ) { IsBackground = true }
                .Start();

            logtid("Waiting task.Result ...");

            Task<int> task = tcs.Task; // Our "slave" task.
            logtid($"{funcname} Wait done. We see task.Result={task.Result}"); // 42            
        }

        /// <summary>
        /// [CSNUT7] p583-584: Our own implementation of Task.Run()
        /// </summary>
        /// <typeparam name="TResult"></typeparam>
        /// <param name="function"></param>
        /// <returns></returns>
        static Task<TResult> MyTaskRun<TResult>(Func<TResult> function)
        {
            var tcs = new TaskCompletionSource<TResult>();
            new Thread(() =>
            {
                try { tcs.SetResult(function()); }
                catch (Exception ex) { tcs.SetException(ex); }
            }).Start();
            return tcs.Task;
        }

        static void p584_use_MyTaskRun()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Calling MyTaskRun() to execute a snippet...");

            Task<int> mytask = MyTaskRun(() =>
                {
                    Thread.Sleep(1000);
                    return 42;
                }
            );

            logtid($"Calling(Waiting for) mytask.Result ...");
            int result = mytask.Result;
            logtid($"Got mytask.Result={result}");
        }

        ////

        static Task<int> GetAnswerToLife_TimerDelay(int delayms)
        {
            var tcs = new TaskCompletionSource<int>();

            // Create a timer that fires once in 2000 ms:
            var timer = new System.Timers.Timer(delayms) { AutoReset = false };
            timer.Elapsed += delegate
                {
                    timer.Dispose(); // Chj: must do it?
                    tcs.SetResult(42);
                };
            timer.Start();

            return tcs.Task;
        }

        static void p584_use_GetAnswerToLife_withSimuTask()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Launching GetAnswerToLife task and wait for result...");

            int delayms = 2000;
            var awaiter = GetAnswerToLife_TimerDelay(delayms).GetAwaiter();
            awaiter.OnCompleted(() => 
                logtid($"Timer due. Result: {awaiter.GetResult()}")
                );

            int waitms = delayms + 200;
            logtid($"Deliberate {waitms}ms wait, so that we see awaiter.OnCompleted() before go to next case.");
            Thread.Sleep(waitms);
            logtid($"Deliberate {waitms}ms wait done.");
        }

        static void Main(string[] args)
        {
            p583_TurnThreadIntoTask();

            Console.Out.WriteLine("");
            p584_use_MyTaskRun();

            Console.Out.WriteLine("");
            p584_use_GetAnswerToLife_withSimuTask();
        }
    }
}
