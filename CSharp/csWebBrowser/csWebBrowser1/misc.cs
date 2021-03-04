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
        void LoadUrls()
        {
            string exedir = AppDomain.CurrentDomain.BaseDirectory; // with a trailing backslash
            string inipath_url = Path.Combine(exedir, "urls.ini");
            string[] urls;

            log($"Loading URLs from: {inipath_url}");

            try
            {
                urls = File.ReadAllLines(inipath_url);
            }
            catch (FileNotFoundException  e)
            {
                log($"The ini file cannot be opened.");
                urls = new string[] {"http://localhost:8000"};
            }

            cbxURL.Items.AddRange(urls);
        }
    }
}
