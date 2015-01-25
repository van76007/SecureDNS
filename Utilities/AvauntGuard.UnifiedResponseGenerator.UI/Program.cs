using System;
using System.Collections.Generic;
using System.Text;
using AvauntGuard.UnifiedResponseGenerator.Lib.Services;

namespace AvauntGuard.UnifiedResponseGenerator.UI
{
    class Program
    {
        static void Main(string[] args)
        {
            UnifiedResponseService service = new UnifiedResponseService();
            service.GenerateResponses();

          //  Console.ReadLine();
        }
    }
}
