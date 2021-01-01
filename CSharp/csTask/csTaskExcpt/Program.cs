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

            logtid("p580_TaskThrowExcpt() Now start a Task.");

            Task task = Task.Run(() =>
            {
                logtid("Task executing, now throwing an Exception.");
                throw null;
            });

            logtid($"Task launched. task.Status={task.Status}");
            logtid("Main-thread sleep for 500ms deliberately.");
            Thread.Sleep(500);

            try
            {
                logtid("Calling task.Wait()");
                task.Wait();
            }
            catch (AggregateException aex)
            {
                logtid($"Got Exception. task.Status={task.Status}");
                if (aex.InnerException is NullReferenceException)
                    Console.WriteLine("Got Null!");
                else
                    throw;
            }
        }

        static void p580_NoWait_NoCatch()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Now start a Task.");

            Task task = Task.Run(() =>
            {
                logtid("Task executing, throwing an Exception.");
                throw null;
            });

            logtid("Task launched, main-thread sleep for 500ms.");
            Thread.Sleep(500);

            logtid($"Main-thread checks: task.Result={task.Status}");

            logtid($"Calling task.Dispose(), a must?");
            task.Dispose();

            logtid($"{funcname} End.");
            
            // -- This function runs smoothly without problem.
        }

        static void p580_DoWait_NoCatch()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Now start a Task.");

            Task task = Task.Run(() =>
            {
                logtid("Task executing, throwing an Exception.");
                throw null;
            });

            logtid("Task launched, main-thread sleep for 500ms.");
            Thread.Sleep(500);

            logtid("Calling task.Wait() but NO catch.");
            task.Wait(); // .Wait will throw out execption.
            logtid($"task.Wait() returned, and task.Result={task.Status}");

            logtid($"{funcname} End.");
        }

        static void TaskThrow_with_Conti()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Now start a Task. See whether Conti will execute if task throws.");

            Task task = Task.Run(() =>
            {
                logtid("Task executing, throwing an Exception in 500ms.");
                Thread.Sleep(500);
                throw null;
            });

            var awaiter = task.GetAwaiter();
            awaiter.OnCompleted(() =>
            {
                logtid("Conti executing...");
            });

            logtid("Sleep for 500+50ms");
            Thread.Sleep(500+50);

            logtid($"{funcname} End.");
        }

        static void Main(string[] args)
        {
            p580_TaskThrowExcpt();

            Console.Out.WriteLine("");
            p580_NoWait_NoCatch();

            Console.Out.WriteLine("");
            TaskThrow_with_Conti();

            Console.Out.WriteLine("");
            p580_DoWait_NoCatch(); // this will err with exception, so I place it at end
        }
    }
}

// More to try: p581 TaskScheduler.UnobservedTaskException;
