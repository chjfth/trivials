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
    class Utils
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

