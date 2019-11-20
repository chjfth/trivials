using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cons1
{
    class Program
    {

        static void Main(string[] args)
        {
            int idle = 0; 
#if WOW
            Console.Write("WOW! ");
#endif
#if DONE
            Console.Write("Done: ");
#endif
            Console.WriteLine("Cons1.");
        }
    }
}
