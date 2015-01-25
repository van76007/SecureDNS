namespace MemCacheTest
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using System.Threading;



    class Program
    {
        private static readonly Dictionary<string, DateTime> cache = new Dictionary<string, DateTime>();
        private const string seed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        private const int limit = 10000;
        private static readonly string[] domains = new[] {".net", ".com", ".mu"};



        static void Main()
        {
            Console.Write("Press 'Enter' to start");
            Console.ReadLine();

            for (int i = 1; i <= limit; i++ )
            {
                string domain = RandomDomainName();
                cache.Add(domain, DateTime.Now);
                Console.WriteLine(i + " > " + domain);
            }

            while(Console.ReadLine() != "exit")
            {
                Console.Write("Type 'exit' to quit!");
            }
        }


        private static string RandomDomainName()
        {
            StringBuilder sb = new StringBuilder();

            int domainLength = new Random().Next(15, 20);
            Random random = new Random();

            while(sb.Length < domainLength)
            {
                sb.Append(seed[random.Next(seed.Length)].ToString());
                Thread.Sleep(10);
            }

            return sb + domains[random.Next(domains.Length)];
        }
    }
}
