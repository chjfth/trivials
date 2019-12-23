using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UIbyThread
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            var form = new Form1();
            Thread thread = new Thread(() => UpdateUI(form));
            thread.Start();

            Application.Run(form);
        }

        static void UpdateUI(Form1 form)
        {
            Thread.Sleep(2000);

            form.Text = "New Title";
        }

    }
}
