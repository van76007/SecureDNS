namespace StressTest
{
    using System;
    using System.Text;
    using System.Threading;
    using Core.Domains;



    class Program
    {
        private const string Seed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        private const int Limit = 1000000;
        private static readonly string[] domains = new[] { ".net", ".com", ".mu" };

        static void Main()
        {
            Console.Write("Press 'Enter' to start");
            Console.ReadLine();

            for (int i = 1; i <= Limit; i++)
            {
                string domain = RandomDomainName();
                DomainServices.singleton.SaveMalware(domain, "test", "s1", "desc", "");
                DomainServices.singleton.SaveMalware(domain, "test", "s2", "desc", "");
                DomainServices.singleton.SaveMalware(domain, "test", "s3", "desc", "");
                DomainServices.singleton.SaveMalware(domain, "test", "s4", "desc", "");

                Console.WriteLine(i + " > " + domain);
            }

            while (Console.ReadLine() != "exit")
            {
                Console.Write("Type 'exit' to quit!");
            }


            
        }


        private static string RandomDomainName()
        {
            StringBuilder sb = new StringBuilder();

            int domainLength = new Random().Next(15, 20);
            Random random = new Random();

            while (sb.Length < domainLength)
            {
                sb.Append(Seed[random.Next(Seed.Length)].ToString());
                Thread.Sleep(5);
            }

            return sb + domains[random.Next(domains.Length)];
        }
    }
}
