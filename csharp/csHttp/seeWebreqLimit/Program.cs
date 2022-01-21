using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace seeWebreqLimit
{
    class Program
    {
        static int s_default_timeout = 5000; // millisec

        static DateTime s_last_DateTime = new DateTime(0);

        static void log(string s)
        {
            DateTime nowdt = DateTime.Now;
            int seconds_gap = (int)(nowdt - s_last_DateTime).TotalSeconds;

            if (s_last_DateTime.Ticks > 0 && seconds_gap > 0)
            {
                Console.Out.WriteLine("".PadLeft(Math.Min(10, seconds_gap), '.'));
            }

            string tsprefix = string.Format("[{0:D2}:{1:D2}:{2:D2}.{3:D3}]",
                nowdt.Hour, nowdt.Minute, nowdt.Second, nowdt.Millisecond);
            Console.Out.WriteLine($"{tsprefix}{s}");

            s_last_DateTime = nowdt;
        }

        static void MyHttpRequests_ReaderClose(int clim, string url)
        {
            int i = 0;
            try
            {
                Console.Out.WriteLine("==== WebRequest with Close() ====");
                ServicePointManager.DefaultConnectionLimit = clim;
                Console.Out.WriteLine($"Now set ServicePointManager.DefaultConnectionLimit={clim}");

                int nRequests = clim + 1;
                Console.Out.WriteLine($"Will send {nRequests} HTTP requests.");
                Console.Out.WriteLine($"Will set HttpWebRequest timeout={s_default_timeout}ms");

                for (i = 0; i < nRequests; i++)
                {
                    log($"Creating WebRequest object #{i + 1}.");

                    HttpWebRequest req = (HttpWebRequest) WebRequest.Create(url);
                    req.Timeout = s_default_timeout;

                    log($"#{i + 1} Sending HTTP request ...");
                    HttpWebResponse res = (HttpWebResponse) req.GetResponse();

                    using (res)
                    {
                        log($"#{i + 1} HTTP response header received (StatusCode={res.StatusCode}).");
                        log(" ");
                    }
                }

                log($"Good. All {nRequests} HTTP requests success, no connection leak.");
            }
            catch (WebException e)
            {
                if (e.Status == WebExceptionStatus.Timeout && i>0)
                {
                    log($"PANIC! Second try of HTTP connection SHOULD NOT Timeout!");
                }
                else
                {
                    throw;
                }
            }
        }

        static void MyHttpRequests_ImplicitReaderClose(int clim, string url)
        {
            int i = 0;
            try
            {
                Console.Out.WriteLine("==== WebRequest *implicit* Close() ====");
                ServicePointManager.DefaultConnectionLimit = clim;
                Console.Out.WriteLine($"Now set ServicePointManager.DefaultConnectionLimit={clim}");

                int nRequests = clim + 1;
                Console.Out.WriteLine($"Will send {nRequests} HTTP requests.");
                Console.Out.WriteLine($"Will set HttpWebRequest timeout={s_default_timeout}ms");

                for (i = 0; i < nRequests; i++)
                {
                    log($"Creating WebRequest object #{i + 1}.");

                    HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
                    req.Timeout = s_default_timeout;

                    log($"#{i + 1} Sending HTTP request ...");
                    HttpWebResponse res = (HttpWebResponse)req.GetResponse();

                    // KEY: Read all http response body, to achieve the implicit Close() effect.
                    //
                    Stream rcvStream = res.GetResponseStream();
                    //
                    BinaryReader binreader = new BinaryReader(rcvStream);
                    byte[] rbytes = binreader.ReadBytes(10*1024*1024);
                    log($"HTTP response body all received, {rbytes.Length} bytes.");
                }

                log($"Good. All {nRequests} HTTP requests success, implicit Close() verified.");
            }
            catch (WebException e)
            {
                if (e.Status == WebExceptionStatus.Timeout && i > 0)
                {
                    log($"PANIC! Second try of HTTP connection SHOULD NOT Timeout!");
                }
                else
                {
                    throw;
                }
            }
        }


        static void MyHttpRequests_NoReaderClose(int clim, string url)
        {
            Console.Out.WriteLine("==== WebRequest without Close() ====");
            ServicePointManager.DefaultConnectionLimit = clim;
            Console.Out.WriteLine($"Now set ServicePointManager.DefaultConnectionLimit={clim}");

            int nRequests = clim + 1;
            Console.Out.WriteLine($"Will send {nRequests} HTTP requests.");
            Console.Out.WriteLine($"Will set HttpWebRequest timeout={s_default_timeout}ms");

            for (int i = 0; i < nRequests; i++)
            {
                log($"Creating WebRequest object #{i + 1}.");

                HttpWebRequest req = (HttpWebRequest) WebRequest.Create(url);
                req.Timeout = s_default_timeout;

                log($"#{i + 1} Sending HTTP request ...");
                HttpWebResponse res = (HttpWebResponse) req.GetResponse();

                log($"#{i + 1} HTTP response header received (StatusCode={res.StatusCode}).");
                log(" ");
            }

            log($"PANIC! All {nRequests} responses received. This SHOULD NOT happen!");
        }

        static int Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.Out.WriteLine("Need two parameters. For example:");
                Console.Out.WriteLine("");
                Console.Out.WriteLine("  seeWebreqLimit 3 http://chjhost.com:8000/");
                Console.Out.WriteLine("");
                Console.Out.WriteLine("This will make 3 pending HTTP message-exchange, and we will see the 4th timeout.");
                return 1;
            }

            int clim = int.Parse(args[0]);
            string url = args[1];

            try
            {
                MyHttpRequests_ReaderClose(clim, url);

                Console.Out.WriteLine("");

                MyHttpRequests_ImplicitReaderClose(clim, url);

                Console.Out.WriteLine("");

                MyHttpRequests_NoReaderClose(clim, url);
            }
            catch (WebException e)
            {
                if (e.Status == WebExceptionStatus.Timeout)
                {
                    log("BAD! We encounter WebExceptionStatus.Timeout on recent request. This is bound to connection limit.");
                }
                else // other unexpected error
                {
                    Console.WriteLine(e);
                    throw;
                }
            }

            return 0;
        }
    }
}
