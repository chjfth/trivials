using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Oracle.ManagedDataAccess.Client;

namespace OraConn1
{
    class Program
    {
        static void Main(string[] args)
        {
            test1();
        }

        static void test1()
        {
            // create connection
            OracleConnection con = new OracleConnection();

            // create connection string using builder
            OracleConnectionStringBuilder ocsb = new OracleConnectionStringBuilder();
            ocsb.UserID = "c##chj";
            ocsb.Password = "123";
            ocsb.DataSource = "10.22.3.161/xe";

            con.ConnectionString = ocsb.ConnectionString;
            //
            // connect-string sample:
            //
            //  USER ID=c##chj;PASSWORD=123;DATA SOURCE=10.22.3.161/xe

            try
            {
                con.Open();
                string res = "Connection established (" + con.ServerVersion + ")";
                Console.WriteLine(res);
            }
            catch (OracleException ex)
            {
                Console.Out.WriteLine($"Connection Error:\r\n  {ex.Message}");
            }

        }
    }
}
