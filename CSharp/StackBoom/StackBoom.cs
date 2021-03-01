using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StackBoom
{
    class StackBoom
    {
        static void Recurse_simple(int n)
        {
            Console.WriteLine($"Recurse_simple ({++n})");
            Recurse_simple(n);
        }

        class MyClass
        {
            public int m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;
            public int m11, m12, m13, m14, m15, m16, m17, m18, m19, m20;
        }

        struct MyStruct
        {
            public int m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;
            public int m11, m12, m13, m14, m15, m16, m17, m18, m19, m20;
        }

        static void Recurse_with_class(int n)
        {
            MyClass obj = new MyClass();
            Console.WriteLine($"Recurse_with_class ({++n}) {obj.m1} {obj.m20}");
            Recurse_with_class(n);
        }

        static void Recurse_with_struct(int n)
        {
            MyStruct obj = new MyStruct();
            Console.WriteLine($"Recurse_with_struct ({++n}) ");
            Recurse_with_struct(n);
        }

        static void Recurse_with_class_array(int arraysize, int n)
        {
            MyClass[] objs = new MyClass[arraysize];

            Console.WriteLine($"Recurse_with_class_array ({arraysize}, {++n})");
            Recurse_with_class_array(arraysize, n);
        }


        static void Recurse_with_struct_array(int arraysize, int n)
        {
            MyStruct[] objs = new MyStruct[arraysize];
            Console.WriteLine($"Recurse_with_struct_array ({arraysize}, {++n})");
            Recurse_with_struct_array(arraysize, n);
        }

        static void Main(string[] args)
        {
            int arraysize = 10;

            Console.WriteLine("Select a method to recurse, and see how many recursions " +
                              "it can afford before stack overflows. ");
            Console.WriteLine("[0] Recurse_simple");
            Console.WriteLine("[1] Recurse_with_class");
            Console.WriteLine("[2] Recurse_with_struct");
            Console.WriteLine("[3] Recurse_with_class_array");
            Console.WriteLine("[4] Recurse_with_class_struct");
            Console.Write("Select 0 ~ 4: ");
            int key = Console.Read(); 

            Console.WriteLine();
            if(key==(int)'0')
                Recurse_simple(0);
            else if(key==(int)'1')
                Recurse_with_class(0);
            else if(key==(int)'2')
                Recurse_with_struct(0);
            else if(key==(int)'3')
                Recurse_with_class_array(10, 0);
            else if(key==(int)'4')
                Recurse_with_struct_array(10, 0);
            else
            {
                Console.WriteLine("Bad selection.");
            }
        }
    }
}
