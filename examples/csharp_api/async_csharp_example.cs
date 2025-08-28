using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace CapSolver 
{
    // P/Invoke declarations
    public static class CapNative 
    {
        const string LIB_NAME = "cap_solver";

        [DllImport(LIB_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong cap_solve_pow(string salt, string target);

        [DllImport(LIB_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int cap_solve_challenges(string seed, nuint count, nuint salt_length, 
                                                     nuint difficulty, ulong[] results);
    }

    // Async C# wrapper
    public class AsyncCapSolver
    {
        public static async Task<ulong> SolvePowAsync(string salt, string target)
        {
            return await Task.Run(() => CapNative.cap_solve_pow(salt, target));
        }

        public static async Task<ulong[]> SolveChallengesAsync(string seed, int count, int saltLength, int difficulty)
        {
            return await Task.Run(() => 
            {
                ulong[] results = new ulong[count];
                int result = CapNative.cap_solve_challenges(seed, (nuint)count, (nuint)saltLength, 
                                                           (nuint)difficulty, results);
                return result == 0 ? results : null;
            });
        }

        public static async Task<ulong[]> SolveChallengesWithProgressAsync(string seed, int count, 
                                                                           int saltLength, int difficulty, 
                                                                           IProgress<int> progress = null)
        {
            return await Task.Run(() => 
            {
                ulong[] results = new ulong[count];
                int result = CapNative.cap_solve_challenges(seed, (nuint)count, (nuint)saltLength, 
                                                           (nuint)difficulty, results);
                
                // Simulate progress reporting
                for (int i = 0; i <= count; i++)
                {
                    progress?.Report((i * 100) / count);
                    if (i < count) Task.Delay(10).Wait(); // Small delay for demo
                }
                
                return result == 0 ? results : null;
            });
        }
    }
}

class AsyncProgram 
{
    static async Task Main(string[] args)
    {
        // Example 1: Async single challenge
        try 
        {
            ulong nonce = await AsyncCapSolver.SolvePowAsync("example_salt", "000");
            Console.WriteLine($"nonce: {nonce}");
        }
        catch (Exception e) 
        {
            Console.WriteLine($"error: {e.Message}");
        }

        // Example 2: Async multiple challenges
        try 
        {
            ulong[] solutions = await AsyncCapSolver.SolveChallengesAsync("multi_challenge_token", 5, 16, 3);
            if (solutions != null) 
            {
                Console.Write("solutions: [");
                for (int i = 0; i < solutions.Length; i++) 
                {
                    if (i > 0) Console.Write(", ");
                    Console.Write(solutions[i]);
                }
                Console.WriteLine("]");
            }
        }
        catch (Exception e) 
        {
            Console.WriteLine($"error: {e.Message}");
        }

        // Example 3: High difficulty with progress
        try 
        {
            var progress = new Progress<int>(percent => 
            {
                Console.WriteLine($"progress: {percent}%");
            });

            ulong[] hardSolutions = await AsyncCapSolver.SolveChallengesWithProgressAsync(
                "hard_challenge_token", 3, 32, 4, progress);
                
            if (hardSolutions != null) 
            {
                Console.Write("hard_solutions: [");
                for (int i = 0; i < hardSolutions.Length; i++) 
                {
                    if (i > 0) Console.Write(", ");
                    Console.Write(hardSolutions[i]);
                }
                Console.WriteLine("]");
            }
        }
        catch (Exception e) 
        {
            Console.WriteLine($"error: {e.Message}");
        }
    }
}
