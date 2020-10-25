using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Runtime.ExceptionServices;
using System.Text;

/*
[2020-10-25]
CsvLiner helper class to ease mapping a CSV line to/from user-defined C# class.

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
				int idx = ((csv_column)attr).idx;

				// ensure idx must not exceed CSV-class total fields.
				Debug.Assert(idx < csv_columns);

				// ensure this idx slot not used yet
				Debug.Assert(ufis[idx] == null);

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
		/// Load a real CSV line to generate a new T instance.
		/// </summary>
		/// <param name="csvline">The csvline can have fewer fields than defined in T.</param>
		/// <returns></returns>
		public static T Get(string csvline)
		{
			string[] fields = csvline.Split(',');

			Debug.Assert(fields.Length <= _ufis.Length);

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

			string[] ss = fi_selected.Select(fi => (string)fi.GetValue(uo)).ToArray();

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
					throw new Exception("Fieldname is not valid: " + fieldnames[i]);
			}

			return arret;
		}

		public int[] idx(string[] fieldnames)
		{
			return Idx(fieldnames);
		}

	}

}
