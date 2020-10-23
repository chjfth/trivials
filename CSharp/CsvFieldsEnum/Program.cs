using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// [2020-10-23] Use an enum type as generic-class's type parameter,
// in order to eliminate some repetitive code typing.
// Refs:
// https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/generics/constraints-on-type-parameters#enum-constraints

namespace CsvFieldsEnum
{
    public class CSV_Record<TEnum>
    {
        public int nFields { get; }
        public string[] Fields { get; private set; }

        public CSV_Record()
        {
            var enums = Enum.GetValues(typeof(TEnum));
            nFields = enums.Length;
            
            // Ensure that enums are exactly counted 0,1,2,3... continuously
            Debug.Assert(nFields == (int)enums.GetValue(enums.Length - 1) + 1);
        }
        
        public string HeaderLine
        {
            get
            {
                string s = Enum.GetName(typeof(TEnum), 0);

                for(int i=1; i<nFields; i++)
                {
                    s += "," + Enum.GetName(typeof(TEnum), i);
                }
                return s;
            }
        }

        public string[] Set(string csvline)
        {
            string[] ss = csvline.Split(',');
            Debug.Assert(ss.Length==nFields);
            Fields = ss;
            return Fields;
        }

        public string this[int index]
        {
            get { return Fields[index]; }
        }

        public string this[string key]
        {
            get { return Fields[Enum.Parse(typeof(TEnum), key).GetHashCode()]; }
        }

        public string this[TEnum e]
        {
            get { return Fields[ e.GetHashCode() ]; }
        }

    }

    //////////////////// USAGE EXAMPLE ////////////////////

    enum Purchase { Food, Price, Qty }

    class Program
    {
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
            //Console.Out.WriteLine("pc.Price = {0}", pc.Price);
        }

        static void Main(string[] args)
        {
            Test();
        }
    }
}
