using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Security;
using System.Text;
using System.Threading.Tasks;

namespace CsvLiner
{
    class Program
    {
        static void Main(string[] args)
        {
			Demo_CsvLiner();

			Demo_CsvLiner_Exception();

			Demo_CsvLiner_Utils();

			Console.Out.WriteLine("==== DONE ====");
		}

		/// <summary>
		/// Define this class to describe a CSV line.
		/// Each class field represents a CSV field(CSV column).
		/// The [csv_column] attribute of each class field tells the order of the CSV column.
		///
		/// You can use an int or a string for the order number.
		/// </summary>
		class CRecord
		{
			[csv_column(0)] public string FOOD;

			[csv_column("1")] public string PRICE;
			
			[csv_column(2)]  public string QTY;
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

			CsvLiner<CRecord>.VerifyHeaderLine("FOOD,PRICE,QTY");

			string csvinput1 = "Apple,1.5,100";
			CRecord rec1 = CsvLiner<CRecord>.Get(csvinput1);
			string csvoutput1 = CsvLiner<CRecord>.Put(rec1);

			if (csvoutput1 == csvinput1)
			{
				Console.WriteLine(rec1.FOOD);
				Console.WriteLine(rec1.PRICE);
				Console.WriteLine(rec1.QTY);

				Console.WriteLine("OK. Match.");
			}

			string[] strcols = {"QTY", "PRICE", "FOOD"};
			int[] idxcols = CsvLiner<CRecord>.Idx(strcols);
			Debug.Assert(idxcols[0]==2 && idxcols[1]==1 && idxcols[2]==0);

			// If you are a freak insisting on existing symbols...
			string[] strcols2 = new string[]
			{
				CsvLiner<CRecord>.uso.QTY,
				CsvLiner<CRecord>.uso.PRICE,
				CsvLiner<CRecord>.uso.FOOD,
			};
			int[] idxcols2 = CsvLiner<CRecord>.Idx(strcols2);
			Debug.Assert(idxcols2[0] == 2 && idxcols2[1] == 1 && idxcols2[2] == 0);

			CRecord r2 = CsvLiner<CRecord>.Get("10,Pear", new int[] {2, 0});
			Debug.Assert(r2.FOOD=="Pear" && r2.PRICE=="" && r2.QTY=="10");

			string s2 = CsvLiner<CRecord>.Put(r2, new int[] {2, 0});
			Debug.Assert(s2=="10,Pear");

			//
			// Simplify typing a bit like this:
			//

			var cc = new CsvLiner<CRecord>();
			rec1 = cc.get(csvinput1);
			csvoutput1 = cc.put(rec1);
			Debug.Assert( csvoutput1==csvinput1);

			Debug.Assert(cc.headerLine == headerline);
			Debug.Assert( cc.columns == CsvLiner<CRecord>.Columns());
		}



		class ERecord1
		{
			[csv_column(1)] public string FOOD;
			[csv_column(0)] public string PRICE;
			[csv_column(1)] public string QTY; // ERROR: duplicate column index
		}
		class ERecord2
		{
			[csv_column(0)] public string FOOD;
			[csv_column(1)] public string PRICE;
			[csv_column(3)] public string QTY; // ERROR: index out-of-bound
		}

		/// <summary>
		/// See CsvLinerException in action, when we pass wrong parameters.
		/// </summary>
		static void Demo_CsvLiner_Exception()
		{
			Console.Out.WriteLine("==== Demo_CsvLiner_Exception : ERecord1 ====");
			//
			try
			{
				string headerline = CsvLiner<ERecord1>.HeaderLine();
				Debug.Assert(false);
			}
			catch (CsvLinerException ex)
			{
				// Something undesired HERE! We hope to catch CsvLinerException, but in vain.
				Console.Out.WriteLine(ex.Message + "\r\n");
				Debug.Assert(false);
			}
			catch (TypeInitializationException ex)
			{
				// Actually, we got this:
				Console.Out.WriteLine("Oops! Got TypeInitializationException. \r\n" +
					"This means we give wrong CsvLiner type parameters at compile time.\r\n" +
					"So we must check InnerException to get CsvLinerException.");

				Type inner_exctype = ex.InnerException.GetType(); 
				string inner_message = ex.InnerException.Message;
				Console.Out.WriteLine(inner_exctype.FullName);
				Console.Out.WriteLine(inner_message);
			}

			Console.Out.WriteLine("==== Demo_CsvLiner_Exception : ERecord2 ====");
			//
			try
			{
				string headerline = CsvLiner<ERecord2>.HeaderLine();
				Debug.Assert(false);
			}
			catch (TypeInitializationException ex)
			{
				Console.Out.WriteLine(ex.InnerException.Message);
			}

			Console.Out.WriteLine("==== Demo_CsvLiner_Exception : Too many input columns ====");
			//
			try
			{
				CRecord rec = CsvLiner<CRecord>.Get("Apple,1.5,100,XYZ");
				Debug.Assert(false);
			}
			catch (CsvLinerException ex)
			{
				Console.WriteLine(ex.Message);
			}

			Console.Out.WriteLine("==== Demo_CsvLiner_Exception : Invalid column name ====");
			//
			try
			{
				int[] idxcols = CsvLiner<CRecord>.Idx(new string[] { "QTY", "PriZe" });
				Debug.Assert(false);
			}
			catch (CsvLinerException ex)
			{
				Console.WriteLine(ex.Message);
			}

			try
			{
				CsvLiner<CRecord>.VerifyHeaderLine("--FOOD,PRICE,QTY--");
				Debug.Assert(false);
			}
			catch (CsvLinerException ex)
			{
				Console.WriteLine(ex.Message);
			}

		}

		public static void Demo_CsvLiner_Utils()
		{
			Console.Out.WriteLine("==== Demo_CsvLiner_Utils ====");

			string csvpath_with_header = @"purchase-with-header.csv";
			string csvpath_no_header = @"purchase-no-header.csv";

			List<CRecord> recsH1 = Utils.LoadCsvFile<CRecord>(csvpath_with_header);
			Debug.Assert(recsH1.Count==3);

			List<CRecord> recsH2 = Utils.LoadCsvFile<CRecord>(csvpath_with_header, HeaderCare.Preserve);
			Debug.Assert(recsH2.Count == 4);

			List<CRecord> recsH3 = Utils.LoadCsvFile<CRecord>(csvpath_with_header, 
				HeaderCare.Verify|HeaderCare.Preserve);
			Debug.Assert(recsH3.Count == 4);

			List<CRecord> recsN1 = Utils.LoadCsvFile<CRecord>(csvpath_no_header, HeaderCare.None);
			Debug.Assert(recsN1.Count == 3);

			try
			{
				List<CRecord> recsN2 = Utils.LoadCsvFile<CRecord>(csvpath_no_header, HeaderCare.Verify);
				Debug.Assert(false);
			}
			catch (CsvLinerException)
			{
				Console.Out.WriteLine("OK. HeaderCare.Verify success.");
			}

			// Test writing.

			string copy_with_header = @"copy-with-header.csv";
			Utils.SaveCsvFile(recsH1, copy_with_header, HeaderCare.Preserve, Encoding.UTF8);

			string text1 = File.ReadAllText(csvpath_with_header);
			string text2 = File.ReadAllText(copy_with_header, Encoding.UTF8);
			Debug.Assert(text1==text2);
		}
	}
}
