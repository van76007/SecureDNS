using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace AvauntGuard.ScreenDNS.Client.Test
{
    class Program
    {
        static void Main(string[] args)
        {
//            IPService.Singleton.UpdateClientIP();
//// 
//            List<string> macs = IPService.Singleton.GetMACAddresses(Dns.GetHostName());
//            foreach (string mac in macs)
//            {
//                Console.WriteLine("MAC : " + mac);
//            }

//            foreach (string dns in DNSService.Singleton.GetNameServers())
//            {
//                Console.WriteLine("NameServer : " + dns);
//            }

            Console.WriteLine(IPService.Singleton.GenerateRouterString());

            Console.ReadLine();
        }
    }
}
