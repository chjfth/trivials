using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace csHttpCmd
{
    class Program
    {
        public class MyHttpException : Exception
        {
            public MyHttpException()
            {
            }

            public MyHttpException(string message)
                : base(message)
            {
            }

            public MyHttpException(string message, Exception inner)
                : base(message, inner)
            {
            }
        }

        static void InitEnv()
        {
            ServicePointManager.ServerCertificateValidationCallback = (sender, certificate, chain, sslPolicyErrors) => true;

            ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls
                                                   | SecurityProtocolType.Tls11
                                                   | SecurityProtocolType.Tls12
                                                   | SecurityProtocolType.Ssl3;

            WebRequest.DefaultWebProxy = null;

        }

        static int s_default_timeout = 5000; // millisec

        static DateTime s_last_DateTime = new DateTime(0);

        static void log(string s)
        {
            DateTime nowdt = DateTime.Now;
            int seconds_gap = (int)(nowdt - s_last_DateTime).TotalSeconds;

            if (s_last_DateTime.Ticks > 0 &&  seconds_gap> 0)
            {
                   Console.Out.WriteLine("".PadLeft(Math.Min(10, seconds_gap), '.'));
            }

            string tsprefix = string.Format("[{0:D2}:{1:D2}:{2:D2}.{3:D3}]", 
                nowdt.Hour, nowdt.Minute, nowdt.Second, nowdt.Millisecond);
            Console.Out.WriteLine($"{tsprefix}{s}");

            s_last_DateTime = nowdt;
        }

        static void UseHttpPostMethod(HttpWebRequest req)
        {
            string spost = "Hi!";
            byte[] bytes2post = Encoding.ASCII.GetBytes(spost);
            req.Method = WebRequestMethods.Http.Post;
            req.ContentLength = bytes2post.Length;

            // NOTE:
            // * Inside req.GetRequestStream(), HTTP request header is actually emitted
            //   (to the network).
            // * Inside postStream.Write(), HTTP body is actually emitted.

            using (Stream postStream = req.GetRequestStream()) 
            {
                postStream.Write(bytes2post, 0, bytes2post.Length);
            }
        }

        static void DoHttpClient(string url)
        {
            log("Connecting to " + url);

            HttpWebRequest req = WebRequest.Create(url) as HttpWebRequest;
            req.Timeout = s_default_timeout;

            // UseHttpPostMethod(req); // Tested.

            // ==== Receive HTTP headers ====

            HttpWebResponse res = req.GetResponse() as HttpWebResponse;
            string info = $"req.GetResponse() returns. HTTP headers below:\n";

            foreach (string header in res.Headers)
            {
                info += $"  {header}: {res.Headers[header]}\n";
            }
            log(info);

            // ==== Receive HTTP body ====

            // Note: Content-Length may be -1, so we may not know the real content length in advance,
            // so we have to read the stream piece by piece.

            Stream rcvstream = res.GetResponseStream();
            rcvstream.ReadTimeout = s_default_timeout; // millisec

            byte[] tmpbuf = new byte[64000]; // one-time buffer
            MemoryStream ms = new MemoryStream(); // accumulated buffer
            int nrdtotal = 0;
            int idxpiece = 0;

            while (true)
            {
                int nrd = rcvstream.Read(tmpbuf, 0, tmpbuf.Length);

                if (nrd > 0)
                {
                    idxpiece++;
                    nrdtotal += nrd;

                    ms.Write(tmpbuf, 0, nrd);

                    log($"#{idxpiece} HTTP body +{nrd} bytes (accum {nrdtotal})");

                    Debug.Assert(nrdtotal==ms.Length);
                }
                else // nrd==0
                {
                    break;
                }
            }

            if (res.ContentLength != -1)
            {
                if (res.ContentLength != nrdtotal)
                {
                    info = $"HTTP protocol violation. " +
                           $"Content-Length({res.ContentLength}) does NOT mismatch received body bytes({nrdtotal}).";
                    throw new MyHttpException(info);
                }
            }

            log("Meet end of HTTP body.");
        }

        static int Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.Out.WriteLine("ERROR: Need a URL as parameter.");
                return 1;
            }

            string url = args[0];

            InitEnv();

            try
            {
                DoHttpClient(url);
            }
            catch (MyHttpException e)
            {
                string info = "UNEXPECT! Client-code detected exception:\n" + e.Message;
                log(info);
                return 4;
            }
            catch (Exception e)
            {
                string info = "ERROR: Exception occurred:\n" + e.Message;
                log(info);
                return 4;
            }

            log("Done.");
            return 0;
        }
    }
}
