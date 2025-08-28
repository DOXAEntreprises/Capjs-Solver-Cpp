using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace CapSolver.Web 
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

    // Request/Response models
    public class ChallengeRequest
    {
        public string Token { get; set; } = "";
        public int Count { get; set; } = 30;
        public int SaltLength { get; set; } = 16;
        public int Difficulty { get; set; } = 3;
    }

    public class ChallengeResponse
    {
        public string ClientId { get; set; } = "";
        public string Token { get; set; } = "";
        public ulong[] Solutions { get; set; } = Array.Empty<ulong>();
        public bool Success { get; set; }
        public string Error { get; set; } = "";
    }

    // Web service for CAP solving
    public class CapService
    {
        public async Task<ChallengeResponse> SolveChallengeAsync(string clientId, ChallengeRequest request)
        {
            return await Task.Run(() => 
            {
                try 
                {
                    ulong[] results = new ulong[request.Count];
                    int result = CapNative.cap_solve_challenges(
                        request.Token, 
                        (nuint)request.Count, 
                        (nuint)request.SaltLength,
                        (nuint)request.Difficulty, 
                        results);
                    
                    return new ChallengeResponse
                    {
                        ClientId = clientId,
                        Token = request.Token,
                        Solutions = result == 0 ? results : Array.Empty<ulong>(),
                        Success = result == 0,
                        Error = result != 0 ? "solving failed" : ""
                    };
                }
                catch (Exception e)
                {
                    return new ChallengeResponse
                    {
                        ClientId = clientId,
                        Token = request.Token,
                        Solutions = Array.Empty<ulong>(),
                        Success = false,
                        Error = e.Message
                    };
                }
            });
        }
    }

    // Web API controller
    [ApiController]
    [Route("api/[controller]")]
    public class CapController : ControllerBase
    {
        private readonly CapService _capService;

        public CapController(CapService capService)
        {
            _capService = capService;
        }

        [HttpPost("solve")]
        public async Task<ActionResult<ChallengeResponse>> SolveChallenge([FromBody] ChallengeRequest request)
        {
            string clientId = HttpContext.Connection.RemoteIpAddress?.ToString() ?? "unknown";
            
            Console.WriteLine($"client: {clientId} token: {request.Token} d: {request.Difficulty} " +
                            $"c: {request.Count} s: {request.SaltLength}");

            var response = await _capService.SolveChallengeAsync(clientId, request);
            
            if (response.Success)
            {
                Console.Write($"{clientId} solutions: [");
                for (int i = 0; i < response.Solutions.Length; i++)
                {
                    if (i > 0) Console.Write(", ");
                    Console.Write(response.Solutions[i]);
                }
                Console.WriteLine("]");
            }
            else
            {
                Console.WriteLine($"{clientId} error: {response.Error}");
            }

            return Ok(response);
        }

        [HttpGet("health")]
        public IActionResult Health()
        {
            return Ok(new { status = "healthy", service = "cap-solver" });
        }
    }
}

// Minimal web app example
class WebProgram 
{
    static void Main(string[] args)
    {
        var builder = WebApplication.CreateBuilder(args);
        
        builder.Services.AddControllers();
        builder.Services.AddSingleton<CapSolver.Web.CapService>();
        
        var app = builder.Build();
        
        app.UseRouting();
        app.MapControllers();
        
        Console.WriteLine("CAP Web API running on http://localhost:5000");
        app.Run();
    }
}
