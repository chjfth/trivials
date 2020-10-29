using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.ExceptionServices;
using System.Text;

/*
[2020-10-25] https://github.com/chjfth/trivials/tree/master/CSharp/CsvLiner

CsvLiner helper class to ease mapping a CSV line to/from user-defined C# class.

Limitation: CSV actual field should not contain commas.

	-- Jimm Chen
*/

namespace CsvLiner
{
	/// <summary>
	/// The attribute class that specifies the column order of a CSV field.
	/// </summary>
	public class csv_column : System.Attribute
	{
		/// <summary>
		/// idx represent the order index of a csv-column, counting from 0.
		/// </summary>
		public int idx { get; set; }

		public csv_column(int idx)
		{
			this.idx = idx;
		}

		public csv_column(string sidx)
		{
			this.idx = Int32.Parse(sidx);
		}
	}

	public class CsvLinerException : Exception
	{
		public CsvLinerException()
		{
		}

		public CsvLinerException(string message)
			: base(message)
		{
		}

		public CsvLinerException(string message, Exception inner)
			: base(message, inner)
		{
		}
	}

	/// <summary>
	/// A generic class that maps a CSV record to a concrete C# class,
	/// so that each *CSV field* can be easily accessed via C# *class field*,
	/// with no code redundancy.
	/// </summary>
	/// <typeparam name="T"> a class name representing a complete row of a CSV.
	/// </typeparam>
	class CsvLiner<T> where T : class, new()
	{
		/// <summary>
		/// static FieldInfo[]s for our T.
		/// </summary>
		private static FieldInfo[] _ufis = Init();

		public static T uso; // the static object for some trick

		/// <summary>
		/// Determine all of T's fields and build a FieldInfo[] array for T.
		/// 
		/// On call finish, the array-subscript represents csv column index;
		/// the array element is corresponding C#-class-field info.
		/// So, given a column idx, we know which C#-class-field to get/set.
		/// </summary>
		/// <returns></returns>
		public static FieldInfo[] Init()
		{
			Type utype = typeof(T);
			FieldInfo[] fis = utype.GetFields();

			int csv_columns = fis.Length;
			FieldInfo[] ufis = new FieldInfo[csv_columns];

			foreach (FieldInfo fi in fis)
			{
				Attribute attr = fi.GetCustomAttribute(typeof(csv_column));

				// Get the column order(idx) from our custom attribute named "csv_column".
				int idx = ((csv_column) attr).idx;

				// ensure idx must not exceed CSV-class total fields.
				if (idx >= csv_columns)
				{
					string s = $"Column index for {fi.Name} exceeds column count.\r\n" +
					           $"  Input column index: {idx}\r\n" +
					           $"  Max valid index: {csv_columns - 1}\r\n";
					throw new CsvLinerException(s);
				}

				// ensure this idx slot not used yet
				if (ufis[idx] != null)
				{
					string s = $"Find duplicate column index for the following two fields:\r\n" +
					           $"  {fi.Name}\r\n" +
					           $"  {ufis[idx].Name}\r\n";

					throw new CsvLinerException(s);
				}

				ufis[idx] = fi;
			}

			// Now a trick: I use uso's fields to store its own fieldnames.
			uso = new T();
			foreach (FieldInfo fi in fis)
			{
				fi.SetValue(uso, fi.Name);
			}

			return ufis;
		}

		/// <summary>
		/// Return total column count of the CSV record.
		/// </summary>
		/// <returns></returns>
		public static int Columns()
		{
			return _ufis.Length;
		}

		/// <summary>
		/// Define this instance method, so that user can call it via a CsvLiner instance.
		/// Just a little less typing.
		/// </summary>
		/// <returns></returns>
		public int columns
		{
			get { return Columns(); }
		}

		/// <summary>
		/// Output a header line for the CSV (as described by user class T)
		/// </summary>
		/// <returns>the header line string</returns>
		public static string HeaderLine()
		{
			// Each FieldInfo.Name property is the "field name" of user class T

			string s = String.Join(",", _ufis.Select(fi => fi.Name));
			return s;
		}

		public string headerLine
		{
			get { return HeaderLine(); }
		}

		public static string HeaderLine(int[] selected_columns)
		{
			FieldInfo[] fi_selected = new FieldInfo[selected_columns.Length];

			for (int i = 0; i < selected_columns.Length; i++)
			{
				int idx_column = selected_columns[i];
				fi_selected[i] = _ufis[idx_column];
			}

			string s = String.Join(",", fi_selected.Select(fi => fi.Name));
			return s;
		}

		/// <summary>
		/// Check whether inputline matches correct headerline. If not throw exception.
		/// </summary>
		/// <param name="inputline">the line to verify</param>
		/// <param name="selected_columns">Tells which columns to care. If null, verify all columns.</param>
		public static void VerifyHeaderLine(string inputline, int[] selected_columns = null)
		{
			string correct;
			if (selected_columns == null)
				correct = CsvLiner<T>.HeaderLine();
			else
				correct = CsvLiner<T>.HeaderLine(selected_columns);

			if (correct != inputline)
			{
				string s = $"VerifyHeaderLine() mismatch.\r\n" +
				           $"  Input :  {inputline}\r\n" +
				           $"  Correct: {correct}\r\n";
				throw new CsvLinerException(s);
			}
		}


		/// <summary>
		/// Load a real CSV line to generate a new T instance.
		/// </summary>
		/// <param name="csvline">The csvline can have fewer fields than defined in T.</param>
		/// <returns></returns>
		public static T Get(string csvline)
		{
			string[] fields = csvline.Split(',');

			if (fields.Length > _ufis.Length)
			{
				string s = $"Input CSV line contains too many fields.\r\n" +
				           $"  Input csvline: {csvline}\r\n" +
				           $"  Input fields : {fields.Length}\r\n" +
				           $"  Max allowed  : {_ufis.Length}\r\n";
				throw new CsvLinerException(s);
			}

			T uo = new T();

			int i = 0;
			for (; i < fields.Length; i++)
			{
				_ufis[i].SetValue(uo, fields[i]);
			}

			for (; i < _ufis.Length; i++)
			{
				_ufis[i].SetValue(uo, "");
			}

			return uo;
		}


		public T get(string csvline)
		{
			return Get(csvline);
		}

		/// <summary>
		/// Caller can provide a partial csvline, which contains selected columns.
		/// </summary>
		/// <param name="csvline"></param>
		/// <param name="select_columns"></param>
		/// <returns></returns>
		public static T Get(string csvline, int[] select_columns)
		{
			string[] selected_fields = csvline.Split(',');

			T uo = new T();

			int i = 0;
			for (; i < _ufis.Length; i++)
			{
				_ufis[i].SetValue(uo, "");
			}

			for (i = 0; i < select_columns.Length; i++)
			{
				int idx_column = select_columns[i];
				_ufis[idx_column].SetValue(uo, selected_fields[i]);
			}

			return uo;
		}


		/// <summary>
		/// Convert a T instance into CSV line string.
		/// </summary>
		/// <param name="uo"></param>
		/// <returns>A csvline string, with comma separated fields.</returns>
		public static string Put(T uo)
		{
			string[] ss = _ufis.Select(fi => (string) fi.GetValue(uo)).ToArray();

			return String.Join(",", ss);
		}

		public string put(T uo)
		{
			return Put(uo);
		}

		/// <summary>
		/// Only selectd columns are constructed for a csvline.
		/// </summary>
		/// <param name="uo"></param>
		/// <param name="selected_columns"></param>
		/// <returns></returns>
		public static string Put(T uo, int[] selected_columns)
		{
			FieldInfo[] fi_selected = new FieldInfo[selected_columns.Length];

			for (int i = 0; i < selected_columns.Length; i++)
			{
				int idx_column = selected_columns[i];
				fi_selected[i] = _ufis[idx_column];
			}

			string[] ss = fi_selected.Select(fi => (string) fi.GetValue(uo)).ToArray();

			return String.Join(",", ss);
		}

		/// <summary>
		/// Query column index from field name.
		/// You can query multiple fields at once.
		/// </summary>
		/// <param name="fieldnames"></param>
		/// <returns></returns>
		public static int[] Idx(string[] fieldnames)
		{
			int[] arret = new int[fieldnames.Length];

			for (int i = 0; i < fieldnames.Length; i++)
			{
				int j;
				for (j = 0; j < _ufis.Length; j++)
				{
					if (fieldnames[i] == _ufis[j].Name)
					{
						arret[i] = j;
						break;
					}
				}

				if (j == _ufis.Length)
				{
					string s = $"Input fieldname is not valid: {fieldnames[i]}";
					throw new CsvLinerException(s);
				}
			}

			return arret;
		}

		public int[] idx(string[] fieldnames)
		{
			return Idx(fieldnames);
		}

	}

	//////// Some utility function below ////////

	[Flags]
	public enum HeaderCare
	{
		None = 0,

		/// <summary>
		/// When reading a csv file, Verify the cvs header does exist and is correct.
		/// When writing, no effect.
		/// </summary>
		Verify = 1,

		/// <summary>
		/// When reading a csv file, the csv header line is loaded as a csv data line.
		/// When writing a csv file, the output contains a header line.
		/// </summary>
		Preserve = 2,
	}

	class Utils
	{
		/// <summary>
		/// Load a csv file and return a list of objects of user-given type(T).
		/// </summary>
		/// <typeparam name="T">Path to a csv file.</typeparam>
		/// <param name="csvpath"></param>
		/// <param name="hc">See comments of enum HeaderCare.</param>
		/// <param name="encoding"></param>
		/// <returns></returns>
		public static List<T> LoadCsvFile<T>(string csvpath,
			HeaderCare hc = HeaderCare.None,
			Encoding encoding = null)
			where T : class, new()
		{
			if (encoding == null)
				encoding = System.Text.Encoding.Default;

			string correct_header_line = CsvLiner<T>.HeaderLine();

			var retlist = new List<T>();

			var csvlines = File.ReadLines(csvpath, encoding);

			using (var enumer = csvlines.GetEnumerator())
			{
				if (!enumer.MoveNext())
					return null;

				string line0 = enumer.Current;

				bool is_line0_header = (line0 == correct_header_line) ? true : false;

				if ((hc & HeaderCare.Verify) != 0)
				{
					// Need verify CSV header
					if (!is_line0_header)
					{
						string s = $"CSV file does not have required headerline.\r\n" +
						           $"CSV file:\r\n" +
						           $"  {csvpath}\r\n" +
						           $"Required header line:\r\n" +
						           $"  {correct_header_line}\r\n";
						throw new CsvLinerException(s);
					}
				}

				if ((hc & HeaderCare.Preserve) != 0) // user want to preserve first line
				{
					retlist.Add(CsvLiner<T>.Get(line0));
				}

				if (!is_line0_header) // first line is not a csv header
				{
					retlist.Add(CsvLiner<T>.Get(line0));
				}

				while (enumer.MoveNext())
				{
					retlist.Add(CsvLiner<T>.Get(enumer.Current));
				}

			} // using enumerator

			return retlist;
		}

		public static void SaveCsvFile<T>(List<T> list, string csvpath,
			HeaderCare hc = HeaderCare.None,
			Encoding encoding = null)
			where T : class, new()
		{
			if (encoding == null)
				encoding = System.Text.Encoding.Default;

			string correct_header_line = CsvLiner<T>.HeaderLine();

			using (StreamWriter textwriter = new StreamWriter(csvpath, false, encoding))
			{
				if ((hc & HeaderCare.Preserve) != 0)
				{
					textwriter.WriteLine(correct_header_line);
				}

				foreach (T tobj in list)
				{
					textwriter.WriteLine(CsvLiner<T>.Put(tobj));
				}
			} // using file
		}
	}
}
