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
                    log($"The ini file cannot be opened or is empty.");
                    continue;
                }
            }

            if(urls==null || urls.Length==0)
                urls = new string[] {"http://localhost:8000"};

            cbxURL.Items.AddRange(urls.Where(n => !String.IsNullOrWhiteSpace(n)).ToArray());
            cbxURL.Text = urls[0];
        }

        void Detect_IESoftwareVersion_hardcore()
        {
            lblIESoftVer.Text = "IE software: " + wb1.Version.ToString();

            log($"IE software version: {wb1.Version.Major} ({wb1.Version.ToString()})"); 

            string fp_xua_edge_html = Path.Combine(s_exedir, html_xua_edge_file);

            log("Detecting real IE software version via "+html_xua_edge_file);

            wb_Navigate(fp_xua_edge_html);

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
        }

        void wbevt_Navigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            log($"[event] wb.Navigating\r\n" +
                $"  URL: {e.Url.ToString()}\r\n" +
                $"  TargetFrameName: {e.TargetFrameName}");
        }

        void wbevt_Navigated(object sender,  WebBrowserNavigatedEventArgs e)
        {
            log($"[event] wb.Navigated\r\n" +
                $"  URL: {e.Url.ToString()}");
        }

        void wbevt_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            log($"[event] wb.DocumentCompleted\r\n" +
                $"  URL: {e.Url.ToString()}\r\n" +
                $"  htmldoc.Uri: {wb1.Document.Url.AbsoluteUri}");
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

    }

}
