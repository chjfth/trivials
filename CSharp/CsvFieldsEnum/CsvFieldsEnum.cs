using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

// [2020-10-23] Use an enum type as generic-class's type parameter,
// in order to eliminate some duplicate code typing.
// Refs:
// https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/generics/constraints-on-type-parameters#enum-constraints

namespace CsvFieldsEnum
{
    public class CSV_Record<TEnum>
    {
		/// <summary>
		/// Tells how many fields the CSV record contains.
		/// This value is determined by TEnum.
		/// </summary>
        public int nFields { get; }

		/// <summary>
		/// This records current CSV row contents.
		/// Each element in Fields[] stores a field value of a CSV row.
		/// </summary>
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
            get { return String.Join(",", Enum.GetNames(typeof(TEnum))); }
        }

		/// <summary>
		/// Given a csvline, store each of its field content into `this` object.
		/// The object content can later be fetched by indexer.
		/// </summary>
		/// <param name="csvline"></param>
		/// <returns></returns>
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
            get { return Fields[ e.GetHashCode() ]; } // any alternative to get e's int value?
        }

    }

}
