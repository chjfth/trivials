using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace prjSkeleton
{
    public partial class Form1 : Form
    {
        #region Logger

        static int get_thread_id() { return AppDomain.GetCurrentThreadId(); }
        static int s_mainthread_tid = get_thread_id();

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
        private void log(string s)
        {
//          lock (_locker)
            {
                DateTime nowdt = DateTime.Now;
                int millisec_gap = (int)(nowdt - s_last_DateTime).TotalMilliseconds;
                int seconds_gap = millisec_gap / 1000;
                if (s_last_DateTime.Ticks > 0 && seconds_gap > 0)
                {
                    string text = "".PadLeft(Math.Min(10, seconds_gap), '.');
                    PrintLine(text);
                }

                string tsprefix = string.Format("[{0:D2}:{1:D2}:{2:D2}.{3:D3}{4,-10}] ",
                    nowdt.Hour, nowdt.Minute, nowdt.Second, nowdt.Millisecond, text_elapse(millisec_gap)
                );
                s_last_DateTime = nowdt;

                PrintLine($"{tsprefix}{s}");
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

        void logtid(string s)
        {
            log(string.Format("{0,-11} {1}", tid(), s));
        }

        void PrintLine(string s)
        {
            // Debug.WriteLine(s);
            tboxLog.Invoke(new Action(() =>
            {
                tboxLog.AppendText(s + Environment.NewLine);
            }));
        }

        void assert_main_thread() { Debug.Assert(get_thread_id() == s_mainthread_tid); }
        void assert_not_main_thread() { Debug.Assert(get_thread_id() != s_mainthread_tid); }

        #endregion

        public Form1()
        {
            InitializeComponent();

            tboxStatus.Text = "Click [Navigate] button to Load the web page.";
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            logtid($"Main-thread-id={s_mainthread_tid}");

            string exename = Path.GetFileName(Application.ExecutablePath);
            this.Text = exename;

            wb_PrepareCallbacks();

            CreateLocalHtmls();

            LoadUrlsToComboBox();

            Detect_IESoftwareVersion_hardcore();

            ckbScriptErrorsSuppressed_CheckedChanged(ckbScriptErrorsSuppressed, EventArgs.Empty);
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            wb1.Dispose(); // must I explicitly do this?
        }

        private void btnNavigate_Click(object sender, EventArgs e)
        {
            wb_Navigate(cbxURL.Text);
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            log("wb.Stop()");
            wb1.Stop();
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            this.tboxLog.Text = "";
        }

        private void cbxURL_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                // [2021-03-11] 奇怪: 用 Alt+↓ 弹出下拉列表，按下箭头选中一项，然后回车，
                // 此处代码竟然会被触发两次。why?
                // 搞得我没法将 wb_Navigate() 放在这里了。

//                wb_Navigate(cbxURL.Text);
            }

        }

        private void cbxURL_KeyPress(object sender, KeyPressEventArgs e)
        {
            // We can see e.KeyChar==13 here.
        }

        private void cbxURL_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                // [2021-03-11] 奇怪: 用 Alt+↓ 弹出下拉列表，按下箭头选中一项，然后回车，
                // 此处代码竟然会被触发两次。why?

                wb_Navigate(cbxURL.Text);

                //e.Handled = true; // must?
            }
        }

        private void ckbEvtNavigating_CheckedChanged(object sender, EventArgs e)
        {
            wb_PrepareCallbacks();
        }

        private void ckbEvtNavigated_CheckedChanged(object sender, EventArgs e)
        {
            wb_PrepareCallbacks();
        }

        private void ckbEvtDocumentCompleted_CheckedChanged(object sender, EventArgs e)
        {
            wb_PrepareCallbacks();
        }

        private void ckbProgressChanged_CheckedChanged(object sender, EventArgs e)
        {
            wb_PrepareCallbacks();
        }

        private void ckbWBStatusTextChanged_CheckedChanged(object sender, EventArgs e)
        {
            wb_PrepareCallbacks();
        }

        private void lnkRefresh_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            log("wb.Refresh()");
            wb1.Refresh();
        }

        private void lnkBack_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if(wb1.CanGoBack)
            {
                log("wb.GoBack()");
                wb1.GoBack();
            }
            else
            {
                log("Cannot GoBack() anymore.");
            }
        }

        private void lnkForward_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if (wb1.CanGoForward)
            {
                log("wb.GoForward()");
                wb1.GoForward();
            }
            else
            {
                log("Cannot GoForward() anymore.");
            }
        }

        private void lnkDocumentMode_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if (wb1.Document == null)
            {
                log("No HtmlDocument is loaded, I cannot get documentMode.");
                return;
            }

            // Inject a javascript function into webpage, then execute it and get return value.
            // Hints: https://stackoverflow.com/a/7801574/151453

            var jsCode = @"function inj_ReportDocumentMode()
{ 
    return document.documentMode; 
}";
            wb1.Document.InvokeScript("execScript", new Object[] { jsCode, "JavaScript" });
            
            var docmode_value = wb1.Document.InvokeScript("inj_ReportDocumentMode"); // return type: double
            log($"documentMode = {docmode_value}");
        }

        private void ckbScriptErrorsSuppressed_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox ckb = ((CheckBox)sender);

            if (ckb.Checked)
                wb1.ScriptErrorsSuppressed = true;
            else
                wb1.ScriptErrorsSuppressed = false;
        }

        private void ckbPollReadyState_CheckedChanged(object sender, EventArgs e)
        {
            wb_PrepareCallbacks();
        }

        private void lnkSaveDocument_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if(wb1.Document==null)
                log("wb.Document is null yet, nothing to write.");

            string filename = $"DocumentText-{Utils.NowTimeStr(true)}.txt";
            string filepath = Path.Combine(s_exedir, filename);

            string doctext = wb1.DocumentText;
            byte[] docbytes = Encoding.UTF8.GetBytes(doctext);

            //File.WriteAllText(filepath, doctext, Encoding.UTF8);
            File.WriteAllBytes(filepath, docbytes);

            log($"{doctext.Length}c/{docbytes.Length}b written to {filepath}");
        }

        public struct S_iframe_doc
        {
            public string depths;  // e.g. "TOP.0.1"
            public HtmlDocument htdoc;
        }

        static IEnumerable<S_iframe_doc> Enum_iframe_tree(string start_depths, HtmlDocument start_htdoc)
        {
            if(start_htdoc==null)
                yield break;

            yield return new S_iframe_doc()
            {
                depths = start_depths,
                htdoc = start_htdoc
            };

            int idx = 0;
            foreach (HtmlWindow iframe in start_htdoc.Window.Frames)
            {
                HtmlDocument hdoc = iframe.GetDocumentEx();
                // -- can workaround cross-domain `iframe.Document` UnAuthorizedAccessException problem

                var ienum = Enum_iframe_tree(start_depths+$".{idx}", hdoc);
                foreach (S_iframe_doc ifdoc in ienum)
                {
                    yield return ifdoc;
                }
                idx++;
            }
        }

        private void lnkHtdocGetHashCodes_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if(wb1.Document==null)
                log("wb.Document is null yet.");

            var ar_ifdocs = new List<S_iframe_doc>();
            foreach (S_iframe_doc ifdoc in Enum_iframe_tree("TOP", wb1.Document))
            {
                ar_ifdocs.Add(ifdoc);
            }

            // Align depth string for better printing
            int dlen_max = ar_ifdocs.Max(s => s.depths.Length);
            int hlen_max = ar_ifdocs.Max(s => s.htdoc.GetHashCode().ToString().Length);

            string info = "HtmlDocument GetHashCode-s:\r\n";
            foreach (S_iframe_doc ifdoc in ar_ifdocs)
            {
                string field1 = $"[{ifdoc.depths}]".PadRight(dlen_max+2);
                string field2 = $"{ifdoc.htdoc.GetHashCode()}".PadLeft(hlen_max);

                string s = $"  {field1} {field2} {ifdoc.htdoc.Url.ToString()}\r\n";
                info += s;
            }

            info += $"  Total HtmlDocument count: {ar_ifdocs.Count}";

            log(info);
        }

        private void lnkGetUserAgent_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            string userAgent = wb1.GetHttpUserAgent();

            if (userAgent == null)
            {
                log("wb.Document not ready yet.");
                return;
            }

            log("User-Agent: "+userAgent);
        }
    }
}
