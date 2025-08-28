using System;
using System.Runtime.InteropServices;

namespace CapSolver 
{
    // P/Invoke declarations for the C API
    public static class CapNative 
    {
        const string LIB_NAME = "cap_solver";

        [DllImport(LIB_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong cap_solve_pow(string salt, string target);

        [DllImport(LIB_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int cap_prng_generate(string seed, nuint length, IntPtr output);

        [DllImport(LIB_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int cap_solve_challenges(string seed, nuint count, nuint salt_length, 
                                                     nuint difficulty, ulong[] results);
    }

    // C# wrapper class
    public class CapSolver
    {
        public static ulong SolvePow(string salt, string target)
        {
            return CapNative.cap_solve_pow(salt, target);
        }

        public static string GeneratePrng(string seed, int length)
        {
            IntPtr buffer = Marshal.AllocHGlobal(length + 1);
            try 
            {
                int result = CapNative.cap_prng_generate(seed, (nuint)length, buffer);
                if (result == 0)
                {
                    return Marshal.PtrToStringAnsi(buffer);
                }
                return null;
            }
            finally 
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        public static ulong[] SolveChallenges(string seed, int count, int saltLength, int difficulty)
        {
            ulong[] results = new ulong[count];
            int result = CapNative.cap_solve_challenges(seed, (nuint)count, (nuint)saltLength, 
                                                       (nuint)difficulty, results);
            return result == 0 ? results : null;
        }
    }
}

class Program 
{
    static void Main(string[] args)
    {
        // Example 1: Single challenge
        ulong nonce = CapSolver.SolvePow("test", "00");
        if (nonce != ulong.MaxValue) 
        {
            Console.WriteLine($"nonce: {nonce}");
        }

        // Example 2: PRNG
        string prng = CapSolver.GeneratePrng("test", 16);
        if (prng != null) 
        {
            Console.WriteLine($"prng: {prng}");
        }

        // Example 3: Multiple challenges
        ulong[] results = CapSolver.SolveChallenges("test", 3, 8, 1);
        if (results != null) 
        {
            Console.Write("results: [");
            for (int i = 0; i < results.Length; i++) 
            {
                if (i > 0) Console.Write(", ");
                Console.Write(results[i]);
            }
            Console.WriteLine("]");
        }
    }
}
