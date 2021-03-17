using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Management;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Policy;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace prjSkeleton
{
    public partial class Form1 : Form
    {
        string s_exedir = AppDomain.CurrentDomain.BaseDirectory; // with a trailing backslash

        static string fname_urls_txt = "urls.txt";
        static string fname_urls_user_txt = fname_urls_txt.AppendStemSuffix(".user"); // urls.user.txt
        static string html_template_file = "detect-ie-engine.html";

        static string html_xua_edge_file = "detect-ie-engine-xua-edge.html";

        void CreateLocalHtmls()
        {
            string exedir = AppDomain.CurrentDomain.BaseDirectory; // with a trailing backslash
            string tmplpath = Path.Combine(exedir, html_template_file);

            string tmpltext = File.ReadAllText(tmplpath);

            List<string> file_urls = new List<string> { tmplpath };

            // Generate detect-ie-engine-no-DOCTYPE.html

            string fp_noDOCTYPE = tmplpath.AppendStemSuffix("-no-DOCTYPE");
            string tx_noDOCTYPE = tmpltext.RemoveDOCTYPE();
            File.WriteAllText(fp_noDOCTYPE, tx_noDOCTYPE);

            var arfp_xua_noDOCTYPE = new List<string>();

            // Generate various detect-ie-engine-XUA-xxx.html

            string metasamp = "<meta charset=\"utf-8\">";
            string[] xuas =
            {
                "7", "8", "9", "10", "11",
                "EmulateIE7", "EmulateIE8", "EmulateIE9", "EmulateIE10", "EmulateIE11",
                "edge"
            };
            foreach (string xua in xuas)
            {
                string meta_xua = $"<meta http-equiv=\"X-UA-Compatible\" content=\"IE={xua}\" />";

                string tx_xua = tmpltext.Replace(metasamp, $"{metasamp}\r\n{meta_xua}");
                string fp_xua = tmplpath.AppendStemSuffix("-xua-" + xua);
                File.WriteAllText(fp_xua, tx_xua);

                string tx_xua_noDOCTYPE = tx_xua.RemoveDOCTYPE();
                string fp_xua_noDOCTYPE = tmplpath.AppendStemSuffix("-nDT-xua-" + xua);
                File.WriteAllText(fp_xua_noDOCTYPE, tx_xua_noDOCTYPE);

                file_urls.Add(fp_xua);
                arfp_xua_noDOCTYPE.Add(fp_xua_noDOCTYPE); // add these later
            }

            // Add no-DOCTYPE files at end.
            file_urls.Add(fp_noDOCTYPE);
            file_urls.AddRange(arfp_xua_noDOCTYPE);

            // Add file-urls to urls.txt

            string fp_stock_urls = Path.Combine(exedir, fname_urls_txt);
            List<string> all_urls = new List<string>(File.ReadAllLines(fp_stock_urls));

            foreach (string file_url in file_urls)
            {
                if(all_urls.IndexOf(file_url)==-1)
                    all_urls.Add(file_url);
            }
            File.WriteAllLines(fp_stock_urls, all_urls);
        }

        void LoadUrlsToComboBox()
        {
            // Load from urls.user.txt or urls.txt .

            string inipath_url1 = Path.Combine(s_exedir, fname_urls_user_txt);
            string inipath_url2 = Path.Combine(s_exedir, fname_urls_txt);
            string[] urls = null;

            foreach (string inipath in new string[] {inipath_url1, inipath_url2})
            {
                try
                {
                    log($"Loading URLs from: {inipath}");

                    urls = File.ReadAllLines(inipath);
                    if (urls.Length == 0)
                        throw new FileNotFoundException("Empty file.");

                    log("The ini file is loaded.");
                    break;
                }
                catch (FileNotFoundException)
                {
                    log($"The ini file cannot be opened or is empty. Skip it.");
                    continue;
                }
            }

            if(urls==null || urls.Length==0)
                urls = new string[] {"http://localhost:8000"};

            cbxURL.Items.AddRange(urls.Where(n => !String.IsNullOrWhiteSpace(n)).ToArray());
            //cbxURL.Text = urls[0];

            // Load the stock wbstart.html (it's there only if you have the git repo).
            //
            string fp_wbstart_html = Regex.Replace(s_exedir,
                @"\\csWebBrowser\\.+$",
                @"\csWebBrowser\htmls\wbstart.html");
            cbxURL.Items.Add(fp_wbstart_html);
        }

        void Detect_IESoftwareVersion_hardcore()
        {
            lblIESoftVer.Text = "IE software: " + wb1.Version.ToString();

            log($"IE software version: {wb1.Version.Major} ({wb1.Version.ToString()})"); 

            string fp_xua_edge_html = Path.Combine(s_exedir, html_xua_edge_file);

            log("Detecting real IE software version via "+html_xua_edge_file);

            wb_Navigate(fp_xua_edge_html);

            cbxURL.Text = fp_xua_edge_html;

            // PENDING... Async wait Navigating result

        }

        void wb_PrepareCallbacks()
        {
            wb1.Navigating -= wbevt_Navigating;
            wb1.Navigating += wbevt_Navigating;

            wb1.Navigated -= wbevt_Navigated;
            wb1.Navigated += wbevt_Navigated;

            wb1.DocumentCompleted -= wbevt_DocumentCompleted;
            wb1.DocumentCompleted += wbevt_DocumentCompleted;

            wb1.ProgressChanged -= wbevt_ProgressChanged;
            if(ckbProgressChanged.Checked)
                wb1.ProgressChanged += wbevt_ProgressChanged;

            wb1.NewWindow -= wbevt_NewWindow;
            wb1.NewWindow += wbevt_NewWindow;

            wb1.StatusTextChanged -= wbevt_StatusTextChanged;
            if(ckbWBStatusTextChanged.Checked)
                wb1.StatusTextChanged += wbevt_StatusTextChanged;

            tmrPollReadyState.Enabled = false;
            if (ckbPollReadyState.Checked == true)
            {
                s_rsprev = wb1.ReadyState;

                int pollms = 10;
                int.TryParse(tboxPollReadyStateMs.Text, out pollms);
                if (pollms <= 0)
                    pollms = 10;

                tboxPollReadyStateMs.Text = pollms.ToString();

                log($"Will poll wb.ReadyState every {pollms} millisec. (now {s_rsprev.ToString()})");

                tmrPollReadyState.Interval = pollms;
                tmrPollReadyState.Tick += tmrevt_PollWBReadyState;
                tmrPollReadyState.Enabled = true;
                tmrPollReadyState.Start();
            }
        }

        private static WebBrowserReadyState s_rsprev = WebBrowserReadyState.Uninitialized;

        void tmrevt_PollWBReadyState(object obj, EventArgs e)
        {
            WebBrowserReadyState rsnow = wb1.ReadyState;
            if (rsnow == s_rsprev)
                return;

            log($"[state] wb.ReadyState: {s_rsprev.ToString()} -> {rsnow.ToString()}");

            s_rsprev = rsnow;
        }

        void wbevt_Navigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            log($"[event] wb.Navigating\r\n" +
                $"  URL: {e.Url.ToString()}\r\n" +
                $"  TargetFrameName: {e.TargetFrameName}");
        }

        struct DualUrl
        {
            public string wbURL; // WebBrowser's URL
            public string htURL; // HtmlDocument's URL
        }

        bool IsTopdocEvent(Uri eventUri, out DualUrl durl)
        {
            bool is_topdoc;
            string wbURL = wb1.Document?.Url.AbsoluteUri.ToString();
            string htURL = eventUri.ToString();
            if (wbURL == htURL)
                is_topdoc = true;
            else
                is_topdoc = false;

            durl = new DualUrl() { wbURL=wbURL, htURL=htURL };
            return is_topdoc;
        }

        private HtmlDocument _tmphdoc = null;

        void wbevt_Navigated(object sender,  WebBrowserNavigatedEventArgs e)
        {
            // [2021-03-16] Chj: We check whether this event is from the top-iframe or an child-iframe.
            // If top-iframe, we will update the address-bar.

            DualUrl durl;
            bool is_topdoc = IsTopdocEvent(e.Url, out durl);

            string update_address_bar = is_topdoc ? "(topdoc: updating address bar)" : "";
            log($"[event] wb.Navigated {update_address_bar}\r\n" +
                $"       wb.URL: {durl.wbURL}\r\n" +
                $"  htmldoc.URL: {durl.htURL}");

            if(is_topdoc)
                cbxURL.Text = e.Url.ToString();
        }

        void wbevt_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            DualUrl durl;
            bool is_topdoc = IsTopdocEvent(e.Url, out durl);
            string topdoc = is_topdoc ? "(topdoc)" : "";

            if (wb1.Document!=null)
            {
                log($"[event] wb.DocumentCompleted {topdoc}\r\n" +
                    $"       wb.URL: {durl.wbURL}\r\n" +
                    $"  htmldoc.URL: {durl.htURL}");
            }
            else
            {
                // This can happen when we Navigate to a folder C:\Users\win7evn\AppData\Roaming
                log($"[event] wb.DocumentCompleted {topdoc}\r\n" +
                    $"  URL: {e.Url.ToString()}\r\n" +
                    $"  wb.Document==null");
            }
        }

        void wbevt_ProgressChanged(Object sender, WebBrowserProgressChangedEventArgs e)
        {
            string pct = "N/A";
            if (e.CurrentProgress == -1)
                pct = "Completed";
            else if (e.MaximumProgress > 0)
                pct = $"{(int)(100 * e.CurrentProgress / e.MaximumProgress)}%";

            log($"[event] wb.ProgressChanged {e.CurrentProgress}/{e.MaximumProgress} ({pct})");
        }

        void wbevt_NewWindow(Object sender, CancelEventArgs e)
        {
            if (ckbBlockNewWindow.Checked)
            {
                e.Cancel = true;

                log($"[event] wb.NewWindow (new window pop out blocked)");
            }
            else
            {
                log($"[event] wb.NewWindow");
            }
        }

        void wbevt_StatusTextChanged(object sender, EventArgs e)
        {
            WebBrowser wb = (WebBrowser) sender;
            tboxStatus.Text = wb.StatusText;

            log($"[event] wb.StatusTextChanged\r\n" +
                $"  New-text: {wb.StatusText}");
        }


        void wb_Navigate(string url)
        {
            log($"wb.Navigate()\r\n" +
                $"  URL: {url}");

            wb1.Navigate(url);
        }
    }

    public static class Utils
    {
        public static string AppendStemSuffix(this string infilepath, string suffix)
        {
            // inifilepath = c:\foo\bar.txt
            // suffix = -user
            // return = c:\foo\bar-user.txt

            string dir = Path.GetDirectoryName(infilepath);
            string stem = Path.GetFileNameWithoutExtension(infilepath);
            string _ext = Path.GetExtension(infilepath);
            string ret = Path.Combine(dir, stem + suffix + _ext);
            return ret;
        }

        public static string RemoveDOCTYPE(this string in_text)
        {
            string ret = in_text.Replace("<!doctype html>" + "\r\n", "");
            return ret;
        }

        public static string NowTimeStr(bool need_millisec = false)
        {
            DateTime nowdt = DateTime.Now;
            string strdt = string.Format("{0:D4}{1:D2}{2:D2}.{3:D2}{4:D2}{5:D2}",
                nowdt.Year, nowdt.Month, nowdt.Day, nowdt.Hour, nowdt.Minute, nowdt.Second);

            if(need_millisec)
                strdt += string.Format(".{0:D3}", nowdt.Millisecond);

            return strdt;
        }
    }

}
