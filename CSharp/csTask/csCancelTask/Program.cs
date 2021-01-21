using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace csCancelTask
{
    class MyCancellationToken
    {
        public bool IsCancellationRequested { get; private set; }

        public void Cancel()
        {
            IsCancellationRequested = true;
        }

        public void ThrowIfCancellationRequested()
        {
            if (IsCancellationRequested)
                throw new OperationCanceledException();
        }
    }


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

        async static Task p606_FooAsync(MyCancellationToken cancellationToken)
        {
            Thread.Sleep(200); // Chj: to tear apart 1000ms boundary

            for (int i = 0; i < 10; i++)
            {
                Console.WriteLine(i);
                await Task.Delay(1000);
                cancellationToken.ThrowIfCancellationRequested();
            }
        }

        async static Task p606_LaunchFoo_and_TimedCancel()
        {
            var token = new MyCancellationToken();

            var tskConti = Task.Delay(2000).ContinueWith(ant => 
                token.Cancel()
                );   // Tell it to cancel in two seconds.

            await p606_FooAsync(token);
        }

        static void p606_TestHandCraftedCancel()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Start.");

            Task task = p606_LaunchFoo_and_TimedCancel();

            try
            {
                task.GetAwaiter().GetResult();
            }
            catch (OperationCanceledException e)
            {
                logtid($"OK. OperationCanceledException caught. We know that execution of p606_FooAsync() has been canceled.");

                Debug.Assert(e.CancellationToken.IsCancellationRequested == false); // bcz we're not using System's CancellationToken type.
            }

            logtid($"{funcname} End.");
        }

        ////

        async static Task p607_FooAsync(CancellationToken cancellationToken)
        {
            Thread.Sleep(200); // Chj: to tear apart 1000ms boundary

            for (int i = 0; i < 10; i++)
            {
                Console.WriteLine(i);
                await Task.Delay(1000, cancellationToken);
            }
        }

        async static Task p607_LaunchFoo_and_TimedCancel()
        {
            var cancelSource = new CancellationTokenSource();

            var tskConti = Task.Delay(2000).ContinueWith(ant => 
                cancelSource.Cancel()
                );   // Tell it to cancel in two seconds.

            await p607_FooAsync(cancelSource.Token);
        }

        static void p607_TestCancel()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Start.");

            Task task = p607_LaunchFoo_and_TimedCancel();

            try
            {
                task.GetAwaiter().GetResult();
            }
            catch (OperationCanceledException e)
            {
                logtid($"OK. OperationCanceledException caught. We know that execution of p607_FooAsync() has been canceled.");

                Debug.Assert(e.CancellationToken.IsCancellationRequested==true);
            }

            logtid($"{funcname} End.");
        }

        ////

        async static Task p608_LaunchFoo_AutoTimedCancel()
        {
            int seconds = 5;
            logtid($"Set CancellationTokenSource to auto cancel in {seconds} seconds.");

            var cancelSource = new CancellationTokenSource(seconds*1000);   // This tells it to cancel in 5 seconds
            await p607_FooAsync(cancelSource.Token); // yes, same as p607
        }

        static void p608_TestAutoTimedCancel()
        {
            string funcname = System.Reflection.MethodBase.GetCurrentMethod().Name + "()";
            logtid($"{funcname} Start.");

            Task task = p608_LaunchFoo_AutoTimedCancel();

            try
            {
                task.GetAwaiter().GetResult();
            }
            catch (OperationCanceledException e)
            {
                logtid($"OK. OperationCanceledException caught.");

                Debug.Assert(e.CancellationToken.IsCancellationRequested == true);
            }

            logtid($"{funcname} End.");
        }

        ////

        static void Main(string[] args)
        {
            p606_TestHandCraftedCancel();

            Console.Out.WriteLine("");
            p607_TestCancel();

            Console.Out.WriteLine("");
            p608_TestAutoTimedCancel();
        }
    }
}
