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
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace prjSkeleton
{
    public partial class Form1 : Form
    {
        void LoadUrlsToComboBox()
        {
            string exedir = AppDomain.CurrentDomain.BaseDirectory; // with a trailing backslash
            string inipath_url1 = Path.Combine(exedir, "urls.user.txt");
            string inipath_url2 = Path.Combine(exedir, "urls.txt");
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

            cbxURL.Items.AddRange(urls);
            cbxURL.Text = urls[0];
        }

    }
}
