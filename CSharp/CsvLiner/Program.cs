using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsvLiner
{
    class Program
    {

        static void Main(string[] args)
        {
            for (int i = 0; i < 10; i++)
            {
                Console.Out.WriteLine($"=== Cycle {i+1}");
                Demo_Speedtest_List();
                Demo_Speedtest_Enum();
            }

        }

        static string filepath = @"D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv";

        public class CSVL_Deposit
	    {
		    [csv_column(0)] public string L1NAME;
		    [csv_column(1)] public string PROVINCE;
		    [csv_column(2)] public string CITY;
		    [csv_column(3)] public string BANKCODE12D;
		    [csv_column(4)] public string L2NAME;
		    [csv_column(5)] public string DATETIME;
	    }

        private static void Demo_Speedtest_List()
        {
            int msec_start = Environment.TickCount;

            string[] csvlines = File.ReadAllLines(filepath, Encoding.GetEncoding("GBK"));

            var csvlist = new List<CSVL_Deposit>();

            foreach (string csvline in csvlines)
            {
                csvlist.Add( CsvLiner<CSVL_Deposit>.Get(csvline));
            }

            int msec_used = Environment.TickCount - msec_start;

            int lines = csvlist.Count;
            Console.Out.WriteLine($"[CsvLiner List]{filepath} Loading cost {msec_used} ms , {lines} lines.");
        }

        private static void Demo_Speedtest_Enum()
        {
            int msec_start = Environment.TickCount;

            IEnumerable<string> csvlines = File.ReadLines(filepath, Encoding.GetEncoding("GBK"));

            var csvlist = new List<CSVL_Deposit>();

            foreach (string csvline in csvlines)
            {
                csvlist.Add(CsvLiner<CSVL_Deposit>.Get(csvline));
            }

            int msec_used = Environment.TickCount - msec_start;

            int lines = csvlist.Count;
            Console.Out.WriteLine($"[CsvLiner Enum]{filepath} Loading cost {msec_used} ms , {lines} lines.");
        }

    }
}
/*
[2020-10-27] On AMD 3950X, Win7 VM, I get following result:

=== Cycle 1
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 593 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 391 ms , 146385 lines.
=== Cycle 2
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 453 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 406 ms , 146385 lines.
=== Cycle 3
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 453 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 375 ms , 146385 lines.
=== Cycle 4
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 438 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 391 ms , 146385 lines.
=== Cycle 5
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 437 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 391 ms , 146385 lines.
=== Cycle 6
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 437 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 375 ms , 146385 lines.
=== Cycle 7
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 438 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 437 ms , 146385 lines.
=== Cycle 8
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 438 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 390 ms , 146385 lines.
=== Cycle 9
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 453 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 391 ms , 146385 lines.
=== Cycle 10
[CsvLiner List]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 422 ms , 146385 lines.
[CsvLiner Enum]D:\cruxcode\AutoCrawler\cmbcsub-gbk.csv Loading cost 406 ms , 146385 lines.

 */
