using System;
using System.Collections.Generic;
using System.ComponentModel;
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
    /// <summary>
    /// HTTP related Utils
    /// </summary>
    static partial class Utils
    {
        /// <summary>
        /// For any WebRequest-related Task that does not support timeout or cancelling,
        /// this encapsulation makes it capable of timeout and cancelling. Great idea!
        /// </summary>
        /// <typeparam name="TResult">The type of user task's result. </typeparam>
        /// <param name="webreq">The WebRequest object that has already initiated an ongoing Task.
        ///     On timeout, this function will call webreq.Abort() to enforce cancellation.
        /// </param>
        /// 
        /// <param name="tskOngoing">The ongoing task "associated" with the webreq.
        ///     This is the very Task this function await for.
        /// </param>
        /// 
        /// <param name="ct_user">User provided cancellation token.
        ///     If not given, you will not be able to actively cancel the Task, unless it timeouts.
        /// </param>
        /// 
        /// <param name="timeout_millisec">Timeout milliseconds. Use -1 for infinite.</param>
        /// 
        /// <returns>The same meaning as tskOngoing's original semantics.
        /// Return TResult value on success, or throw an exception on failure.
        /// Will throw TimeoutException on timeout.
        /// </returns>
        public async static Task<TResult> WebRequest_TaskTimeout<TResult>(
            WebRequest webreq, 
            Task<TResult> tskOngoing, // how do I easily recognize webreq & tskOngoing are of the same source?
            CancellationToken ct_user = new CancellationToken(),
            int timeout_millisec = -1
        )
        {
            // TResult maybe: WebRespone, Stream(from WebRequest.GetResponseStream()), etc

            if (timeout_millisec < 0)
                timeout_millisec = -1; // Task.Delay() recognize -1 as infinite

            var cts_inner = new CancellationTokenSource(); 
            // -- use this to solely cancel our internal Delay Task.
            // Cancellation of cts_inner will not affect user's own cancellation token.

            ct_user.Register(() =>
                cts_inner.Cancel() // chain cancellation
            );

            Task tskTimeout = Task.Delay(timeout_millisec, cts_inner.Token);

            Task tskCompleted = await Task.WhenAny(tskOngoing, tskTimeout);

            if (tskCompleted == tskOngoing)
            {
                // We should explicitly cancel our now-useless internal Delay Task,
                // in a timely manner, so that associated memory is freed.
                cts_inner.Cancel();
                return tskOngoing.Result;
            }
            else
            {
                // Timeout. So we need to cancel(abort) our "true" Task in turn.

                //// Thread.Sleep(1000); // Enable this in hope to see Abort() and success result.

                // Hope this aborts EVERYTHING, inc DNS resolving, TCP connect,
                // SSL certificate verification etc.

                webreq.Abort();

                // Here, distinguish whether we report to user Cancelled or Timed-out.
                if (tskTimeout.Status == TaskStatus.RanToCompletion)
                {
                    string info = $"Whoa! WebRequest operation timed out after {timeout_millisec} milliseconds.\r\n" +
                                  $"URL: {webreq.RequestUri}";
                    throw new TimeoutException(info);
                }

                // We still need to wait for the true result of the webreq.
                // This may give our success result,
                // or throw an WebException with WebExceptionStatus.RequestCanceled .
                TResult result = await tskOngoing;

                return result;
            }
        }

        public async static Task WebRequest_TaskTimeout(
            WebRequest webreq,
            Task tskOngoing,
            CancellationToken ct_user,
            int timeout_millisec
        )
        {
            Task<object> tskhelper = TaskT_FromTask(tskOngoing);
            await WebRequest_TaskTimeout(webreq, tskhelper, ct_user, timeout_millisec);
        }

        public static Task<object> TaskT_FromTask(Task origtask)
        {
            // Hint from [CSNUT7] p613

            var tcs = new TaskCompletionSource<object>(); // mants: manual Task source

            origtask.ContinueWith(antecedent =>
            {
                if (antecedent.IsCanceled)
                    tcs.TrySetCanceled();
                else if (antecedent.IsFaulted)
                    tcs.TrySetException(antecedent.Exception.InnerException);
                else
                    tcs.TrySetResult(null);
            });

            return tcs.Task;
        }

    }



    /// <summary>
    /// MessageBox Utils
    /// </summary>
    static partial class Utils
    {
        public static void ErrorMsg(string err)
        {
            MessageBox.Show(err, "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        public static void WarnMsg(string msg)
        {
            MessageBox.Show(msg, "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
        public static void InfoMsg(string err)
        {
            MessageBox.Show(err, "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        public static bool YesOrNo(string s, MessageBoxIcon msgicon = MessageBoxIcon.Question)
        {
            var ret = MessageBox.Show(s, "询问", MessageBoxButtons.YesNo, msgicon);
            return ret == DialogResult.Yes ? true : false;
        }

    }
}

