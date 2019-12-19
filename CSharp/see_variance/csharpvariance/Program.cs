using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace csharpvariance
{
    class Program
    {
        //////////////// CASE 0 ////////////////

        class A { }
        class B : A { }
        class C : A { }

        static void test_case0()
        {
            A a = null;
            B b = null;
            a = b;
        }

        //////////////// CASE 1 ////////////////

        interface Compound1<out T> // `out` is required to pass compile
        {
            T Fetch1();
            T Fetch2();
        }

        interface Compound2<in T> // `in` is required to pass compile
        {
            void Print1(T fmt);
            void Print2(T fmt);
        }

        static void test_case1()
        {
            Compound1<A> c1a = null;
            Compound1<B> c1b = null;
            c1a = c1b; // view Compound1<B> object as a Compound1<A> one

            Compound2<A> c2a = null;
            Compound2<B> c2b = null;
            c2b = c2a; // view Compound2<A> object as a Compound2<B> one
        }

        //////////////// CASE 2 ////////////////

        delegate T GETOBJ<out T>();
        delegate void USEOBJ<in T>(T obj);

        static void test_case2()
        {
            GETOBJ<A> GetA = null;
            GETOBJ<B> GetB = null;

            // Although GetB() returns a B-type, we can consider it returns an A-type.
            // We do the latter via GetA.
            GetA = GetB; 

            USEOBJ<A> UseA = null;
            USEOBJ<B> UseB = null;

            // Although UseA() accepts an A-type, we can use it as if it accepts a B-type.
            // We do the latter via GetB.
            UseB = UseA; 
        }

        static void Main(string[] args)
        {
        }
    }
}
