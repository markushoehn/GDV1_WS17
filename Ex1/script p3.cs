// compile on

/////////////////////////////////////////////////
// http://rextester.com                        //
/////////////////////////////////////////////////


//Rextester.Program.Main is the entry point for your code. Don't change it.
//Compiler version 4.0.30319.17929 for Microsoft (R) .NET Framework 4.5

using System;

namespace Rextester
{
    public class Program
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("f1(5223): " + f1(5223));
            Console.WriteLine("f2(45): " + f2(45));
            Console.WriteLine("f2(77): " + f2(77));
            Console.WriteLine("f3(15, 30): " + f3(15, 30));
            Console.WriteLine("f4(40, 100): " + f4(40, 100));
            Console.WriteLine("f5(67, 5730): " + f5(67, 5730).ToString());
        }
        
        private static double f1(int p)
        {
            return 180 / Math.PI * Math.Atan(Math.Abs(((double)p / 8191) * 48 - 24) / 24);
        }
        
        private static double f2(double gamma)
        {
           return 2 * gamma - 90;
        }
        
        private static double f3(double alpha, double beta)
        {
            alpha = alpha * Math.PI / 180;
            beta = beta * Math.PI / 180;
            
            return Math.Cos(alpha) * (150 * Math.Sin(Math.PI/2 -beta)) / Math.Sin(alpha + beta);
        }
        
        private static double f4(double beta, double z)
        {
            beta = beta * Math.PI / 180;
            return Math.Tan(beta) * z;
        }
        
        private static P f5(double gamma, int p)
        {
            return new P{X = f4(f1(p), f3(f2(gamma), f1(p))),
                         Z = f3(f2(gamma), f1(p))};
        }
        
        private class P
		{
            public double X;
            public double Z;
            
            public override string ToString()
            {
                return "(x = " + X + ", z = " + Z + ")";
            }
        }
    }
}