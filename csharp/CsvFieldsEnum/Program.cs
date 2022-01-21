using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsvFieldsEnum
{
	enum Purchase { Food, Price, Qty }

    class Program
    {
	    static void Main(string[] args)
	    {
		    Test();
	    }

		static void Test()
        {
            var pc = new CSV_Record<Purchase>();

            string s = pc.HeaderLine;
            Console.Out.WriteLine("HeaderLine = {0}", s);

            Console.Out.WriteLine("nFields = {0}", pc.nFields);

            pc.Set("Apple,2.5,100"); // this string is a line from csv file

            Console.Out.WriteLine("pc[0] = {0}", pc[0]);

            Console.Out.WriteLine("pc[Purchase.Food] = {0}", pc[Purchase.Food]);
            Console.Out.WriteLine("pc[Purchase.Price] = {0}", pc[Purchase.Price]);
            Console.Out.WriteLine("pc[Purchase.Qty] = {0}", pc[Purchase.Qty]);

            Console.Out.WriteLine("pc[\"Food\"] = {0}", pc["Food"]);
            Console.Out.WriteLine("pc[\"Price\"] = {0}", pc["Price"]);
            Console.Out.WriteLine("pc[\"Qty\"] = {0}", pc["Qty"]);

			// Q: Can I go one step further?
			// Console.Out.WriteLine("pc.Price = {0}", pc.Price);
			// [2020-10-26] Yes, check out my CsvLiner demo project.
		}

	}
}
