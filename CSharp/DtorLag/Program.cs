using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DtorLag
{
    class Work
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

        private int _lagsec_on_quit = 0;
        private string _name;

        public Work(string name, int lagsec)
        {
            _name = name;
            _lagsec_on_quit = lagsec;
        }

        ~Work()
        {
            logtid($"{_name} Finalizer Start. >>>");

            DoLag(_lagsec_on_quit, true);

            logtid($"{_name} Finalizer Done. <<<");
        }

        public void DoLag(int sec, bool is_from_dtor=false)
        {
            string dtor_mark = is_from_dtor ? "*" : "";
            DateTime dt_start = DateTime.Now;
            DateTime dt_lastprint = dt_start- TimeSpan.FromSeconds(1);

            while (true)
            {
                DateTime dt_now = DateTime.Now;
                
                if ((dt_now - dt_start) >= TimeSpan.FromSeconds(sec))
                    break;

                if (dt_now - dt_lastprint >= TimeSpan.FromMilliseconds(500))
                {
                    logtid($"{_name}{dtor_mark}: lagging...");
                    dt_lastprint = dt_now;
                }
            }

            logtid($"{_name}{dtor_mark}: lagging done.");
        }
    }

    class Program
    {
        static Work CreateWorks(int lagsec)
        {
            Work wa = new Work("Work A", lagsec);
            Work wb = new Work("Work B", lagsec);

            return wa;
        }

        static void Main(string[] args)
        {
            int lagsec = 5;
            if (args.Length == 1)
                lagsec = int.Parse(args[0]);

            Console.Out.WriteLine($"Lag seconds is {lagsec}");

            Work wa = CreateWorks(lagsec);

            GC.Collect();

            wa.DoLag(lagsec);

            Console.Out.WriteLine($"Main() return.");
        }
    }
}
