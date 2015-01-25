using System;
using System.Collections.Generic;
using System.ServiceProcess;
using System.Text;

namespace ScreenDNS_Client
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
            ServiceBase[] ServicesToRun;
            ServicesToRun = new ServiceBase[] 
			{ 
				new ScreenDNSService() 
			};
            ServiceBase.Run(ServicesToRun);
        }
    }
}
