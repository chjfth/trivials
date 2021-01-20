﻿using System;
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
                _webeq_SetHeader_ObeyRule(header.Key, header.Value);
            }

            _postbody_text = postbodyText;
            if (_postbody_text == "")
                _postbody_text = null;

            if(_postbody_text!=null)
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

        public string _url { get; private set; }

        public string _postbody_text { get; private set; }

        public byte[] _postbody_bytes { get; private set; }

        public byte[] _respbody_bytes { get; private set; }

        public string _respbody_text { get; private set; }

        private HttpWebRequest _webreq;

        public HttpWebRequest webreq
        {
            get { return _webreq;  }
            set { _webreq = value; }
        }

        public async Task<string> StartAsText(CancellationToken ct, int timeout_millisec)
        {
            byte[] bodybytes = await Start(ct, timeout_millisec);

            if (bodybytes != null)
            {
                _respbody_text = Encoding.UTF8.GetString(bodybytes);
                return _respbody_text;
            }
            else
                return null;
        }

        public async Task<byte[]> Start(CancellationToken ct, int timeout_millisec)
        {
            Task tskTimeout = Task.Delay(TimeSpan.FromMilliseconds(timeout_millisec), ct);

            if (_webreq.Method == WebRequestMethods.Http.Post)
            {
                Stream postStream = await _webreq.GetRequestStreamAsync(); // todo cts
                // -- This initiates an http connection to the server, and if connect success,
                // the http headers will be sent to server automatically.

                using (postStream)
                {
                    // Closing postStream tells the system we have POSTed all http body bytes.

                    await postStream.WriteAsync(_postbody_bytes, 0, _postbody_bytes.Length, ct);
                }
            }

            // WebRequest.GetResponseAsync() does not support Cancellation, 
            // so we use a Delay Task to help implement the timeout. Works very well.

            Task<WebResponse> tskWeb = _webreq.GetResponseAsync();

            Task tskCompleted = await Task.WhenAny(tskWeb, tskTimeout);

            if (tskCompleted == tskWeb)
            {
                WebResponse webresp = tskWeb.Result;
                using (webresp)
                {
                    // Get HTTP response body Stream object, so that we can read the response body
                    // piece by piece from the Stream object.
                    // Note: There is no API called WebResponse.GetResponseStreamAsync(), bcz 
                    // GetResponseStream() never block, and slow reading of http body is deferred to
                    // stream reading.

                    Stream httpbodys = webresp.GetResponseStream();

                    // Since we cannot know in advance how long the http body is, especially for
                    // "chunked" transferring, so we need a temp stream as our own buffer.
                    MemoryStream tempbodys = new MemoryStream();

                    Task tskRecvBody = httpbodys.CopyToAsync(tempbodys, 81920, ct);

                    tskCompleted = await Task.WhenAny(tskRecvBody, tskTimeout);

                    if (tskCompleted == tskRecvBody)
                    {
                        _respbody_bytes = tempbodys.GetBuffer();
                        return _respbody_bytes;
                    }
                    else
                    {
                        // Receiving body timeout
                        _webreq.Abort();
                        await tskRecvBody; 

                        // todo: Save partial recv body
                        _respbody_bytes = tempbodys.GetBuffer();
                        return null;
                    }
                }
            }
            else
            {
// TODO: add logging facility to know the detailed timing
                // Timeout. So we need to cancel(Abort) our "true" Task in turn.

                // Hope this aborts EVERYTHING, inc DNS resolving, TCP connect,
                // SSL certificate verification etc.
                _webreq.Abort();

                // The calm down may not be instant, so we still need to wait for true 
                // WebRequest task finish.
                await tskWeb;

                return null;
            }

        }

        #region AUX

        void _webeq_SetHeader_ObeyRule(HttpRequestHeader hdr, string value)
        {
            // If not obeying the rule, .NET throw System.ArgumentException sth like this:
            // The 'User-Agent' header must be modified using the appropriate property or method. 
            switch (hdr)
            {
                case HttpRequestHeader.Accept:
                    _webreq.Accept = value;
                    break;
                case HttpRequestHeader.ContentType:
                    _webreq.ContentType = value;
                    break;
                case HttpRequestHeader.Expect:
                    _webreq.Expect = value;
                    break;
                case HttpRequestHeader.Host:
                    _webreq.Host = value;
                    break;
                case HttpRequestHeader.Referer:
                    _webreq.Referer = value;
                    break;
                case HttpRequestHeader.UserAgent:
                    _webreq.UserAgent = value;
                    break;
                default:
                    _webreq.Headers[hdr] = value;
                    break;
            }
        }

        #endregion
    }
}

// todo:
// ServicePointManager.ServerCertificateValidationCallback = (sender, certificate, chain, sslPolicyErrors) => true;