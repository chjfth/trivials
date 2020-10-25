using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsvLiner
{
    class Program
    {
        static void Main(string[] args)
        {
			Demo_CsvLiner();
        }

		/// <summary>
		/// Our CSV contains 3 fields(columns).
		/// We use enum symbols to represent their order (although optional).
		/// So Food=0, Price=1, Qty=2.
		/// </summary>
        enum ColIdx { Food, Price, Qty }

		/// <summary>
		/// Define this class to describe a CSV line.
		/// Each class field represents a CSV field(CSV column).
		/// The [csv_column] attribute of each class field tells the order of the CSV column.
		///
		/// You can use an int or a string for the order number.
		/// </summary>
		class CRecord
		{
			[csv_column(0)]  public string Food;

			[csv_column("1")] public string Price;
			
			[csv_column((int)ColIdx.Qty)]  public string Qty;
		}

		/// <summary>
		/// Demo code.
		/// You see, we can use natural C# obj.field syntax to access each CSV field,
		/// no need to touch any ugly/verbose/redundant number for csv column index.
		/// What a breeze!
		/// </summary>
		static void Demo_CsvLiner()
		{
			string headerline = CsvLiner<CRecord>.HeaderLine();
			Console.WriteLine(headerline);

			string csvinput1 = "Apple,1.5,100";
			CRecord rec1 = CsvLiner<CRecord>.Get(csvinput1);
			string csvoutput1 = CsvLiner<CRecord>.Put(rec1);

			if (csvoutput1 == csvinput1)
			{
				Console.WriteLine(rec1.Food);
				Console.WriteLine(rec1.Price);
				Console.WriteLine(rec1.Qty);

				Console.WriteLine("OK. Match.");
			}

			string[] strcols = {"Qty", "Price", "Food"};
			int[] idxcols = CsvLiner<CRecord>.Idx(strcols);
			Debug.Assert(idxcols[0]==2 && idxcols[1]==1 && idxcols[2]==0);

			// If you are a freak insisting on existing symbols...
			string[] strcols2 = new string[]
			{
				CsvLiner<CRecord>.uso.Qty,
				CsvLiner<CRecord>.uso.Price,
				CsvLiner<CRecord>.uso.Food,
			};
			int[] idxcols2 = CsvLiner<CRecord>.Idx(strcols2);
			Debug.Assert(idxcols2[0] == 2 && idxcols2[1] == 1 && idxcols2[2] == 0);

			//
			// Simplify typing like this:
			//

			var cc = new CsvLiner<CRecord>();
			rec1 = cc.get(csvinput1);
			csvoutput1 = cc.put(rec1);
			Debug.Assert( csvoutput1==csvinput1);

			Debug.Assert(cc.headerLine == headerline);
			Debug.Assert( cc.columns == CsvLiner<CRecord>.Columns());
		}
	}
}
