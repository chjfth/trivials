using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZjbLib
{
    class AsyncHttp
    {
        public class HeaderDict : Dictionary<HttpRequestHeader, string>
        {
        }

        public AsyncHttp(string url, 
            HeaderDict headers, //Dictionary<HttpRequestHeader, string> headers,
            string postbodyText = null
            )
        {
            _url = url;
            _webreq = WebRequest.Create(_url) as HttpWebRequest;

            foreach (var header in headers)
            {
                _webreq.Headers[header.Key] = header.Value;
            }

            _postbody_text = postbodyText;
            if (_postbody_text == "")
                _postbody_text = null;

            if(_postbody_text==null)
                _postbody_bytes = Encoding.UTF8.GetBytes(_postbody_text);

            //
            // Determine some HTTP request params automatically
            //

            // == Determine GET or POST ==
            //
            if (_postbody_bytes == null)
                _webreq.Method = WebRequestMethods.Http.Get;
            else
                _webreq.Method = WebRequestMethods.Http.Post;

            // == Determine Content-Length ==
            //
            if (_postbody_bytes != null)
                _webreq.ContentLength = _postbody_bytes.Length;

        }

        private string _url;
        private HttpWebRequest _webreq;
        private string _postbody_text;
        private byte[] _postbody_bytes;

        private byte[] _respbody_bytes;

        public HttpWebRequest webreq
        {
            get { return _webreq;  }
            set { _webreq = value; }
        }

        public async Task<byte[]> Start(CancellationToken ct, int timeout_millisec)
        {
            try
            {
                Stream postStream = await _webreq.GetRequestStreamAsync(); // todo cts
                // -- This initiates an http connection to the server, and if connect success,
                // the http headers will be sent to server automatically.

                using (postStream)
                {
                    // Closing postStream tells the system we have POSTed all http body bytes.

                    await postStream.WriteAsync(_postbody_bytes, 0, _postbody_bytes.Length, ct);
                }

                WebResponse webres = await _webreq.GetResponseAsync();
                using (webres)
                {
                    // Get HTTP response body Stream object, so that we can read the response body
                    // piece by piece from the Stream object.
                    // Note: There is no API called WebResponse.GetResponseStreamAsync(), bcz 
                    // GetResponseStream() never block, and slow reading of http body is deferred to
                    // stream reading.

                    Stream httpbodys = webres.GetResponseStream();

                    // Since we cannot know in advance how long the http body is, especially for
                    // "chunked" transferring, so we need a temp stream as our own buffer.
                    MemoryStream tempbodys = new MemoryStream();
                    await httpbodys.CopyToAsync(tempbodys, 81920, ct);

                    _respbody_bytes = tempbodys.GetBuffer();
                    return _respbody_bytes;
                }
            }
            catch (WebException e)
            {
                Console.WriteLine(e);
                throw;
            }

//            return _respbody_bytes;
        }
    }
}

// todo:
// ServicePointManager.ServerCertificateValidationCallback = (sender, certificate, chain, sslPolicyErrors) => true;