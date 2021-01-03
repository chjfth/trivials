using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace csCancelTask
{
    class CancellationToken
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

        async static Task p606_FooAsync(CancellationToken cancellationToken)
        {
            for (int i = 0; i < 10; i++)
            {
                Console.WriteLine(i);
                await Task.Delay(1000);
                cancellationToken.ThrowIfCancellationRequested();
            }
        }

        async static Task p606_LaunchFoo_and_TimedCancel()
        {
            var token = new CancellationToken();

            Task.Delay(2000).ContinueWith(
                ant => token.Cancel()
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
            }

            logtid($"{funcname} End.");
        }

        static void Main(string[] args)
        {
            p606_TestHandCraftedCancel();
        }
    }
}
