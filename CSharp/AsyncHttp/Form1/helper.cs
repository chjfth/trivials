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
    /// <summary>
    /// HTTP related Utils
    /// </summary>
    static partial class Utils
    {
        /// <summary>
        /// For any WebRequest-related Task that does not support timeout or cancelling,
        /// this encapsulation makes it capable of timeout and cancelling. Great idea.
        /// </summary>
        /// <typeparam name="TResult"></typeparam>
        /// <param name="webreq"></param>
        /// <param name="tskOngoing"></param>
        /// <param name="ct"></param>
        /// <param name="timeout_millisec"></param>
        /// <returns>The same meaning as tskOngoing's original semantics.
        /// Return TResult value on success, or throw an exception on failure.</returns>
        public async static Task<TResult> WebRequest_TaskTimeout<TResult>(
            WebRequest webreq, 
            Task<TResult> tskOngoing,
            CancellationToken ct,
            int timeout_millisec
        )
        {
            // TResult maybe: WebRespone, ...

            Task tskTimeout = Task.Delay(TimeSpan.FromMilliseconds(timeout_millisec), ct);

            Task tskCompleted = await Task.WhenAny(tskOngoing, tskTimeout);

            if (tskCompleted == tskOngoing)
            {
                return tskOngoing.Result;
            }
            else
            {
                // Timeout. So we need to cancel(abort) our "true" Task in turn.

                // Hope this aborts EVERYTHING, inc DNS resolving, TCP connect,
                // SSL certificate verification etc.
                webreq.Abort();

                // We still need to wait for the true result of the webreq.
                // This may give our success result,
                // or an WebException with WebExceptionStatus.RequestCanceled .
                TResult result = await tskOngoing;
                return result;
            }
        }

        public async static Task WebRequest_TaskTimeout(
            WebRequest webreq,
            Task tskOngoing,
            CancellationToken ct,
            int timeout_millisec
        )
        {
            Task<object> tskhelper = TaskTFromTask(tskOngoing);
            await WebRequest_TaskTimeout(webreq, tskhelper, ct, timeout_millisec);
        }

        public static Task<object> TaskTFromTask(Task origtask)
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

