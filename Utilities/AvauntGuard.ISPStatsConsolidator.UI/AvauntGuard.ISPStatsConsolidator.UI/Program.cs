using System;
using System.Collections.Generic;
using System.Text;
using AvauntGuard.ISPStatsConsolidator.Lib.Services;
using AvauntGuard.ISPStatsConsolidator.Lib.Objects;
using System.Threading;
using AvauntGuard.MailSender.Lib.Services;
using System.Configuration;
using System.Text.RegularExpressions;

namespace AvauntGuard.ISPStatsConsolidator.UI
{
    class Program
    {
        static void Main(string[] args)
        {
            DoTest();

            // ConsolidateStatistics();            

            // ISPStatsConsolidatorService.Singleton.CalculateDailyStats(new DateTime(2011, 11, 16), false, "ans_ip");
        }

        static void DoTest()
        {
          //  ISPStatsConsolidatorService.Singleton.BuildSourceFireVRT("http://labs.snort.org/iplists/", CountryCode.MU, string.Format("dnslist-{0}", DateTime.Now.AddDays(-1 * 5).ToString("yyyy-MM-dd")));
                   

            //for (int i = 0; i <= 150; i++)
            //{
            //    Console.WriteLine("Day is " + (-1 * i));
            //    ISPStatsConsolidatorService.Singleton.CalculateDailyStats(DateTime.Today.AddDays((-1 * i)), false, "ans_ip");
            //}

            Console.WriteLine("DONE");
            Console.ReadLine();
        }

        static void ConsolidateStatistics()
        {
            bool buildInfectionStats = true;// true;

            // Not required anymore, done in real time
            bool updateBlockedDomainIPs = false;

            bool performDatabaseCleanup = true;// true;
            bool performWhitelistCleanup = true;// true;
            bool buildStatistics = true;

            StringBuilder sb = new StringBuilder();

            sb.AppendLine("<font>=========================================<br/>");
            sb.AppendLine(String.Format("= RSS parsing started at : {0}.<br/>", DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss tt")));
            sb.AppendLine("=========================================<br/>");

            if (buildInfectionStats)
            {
                try
                {
                    // Total Harvest
                    ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromRSS("http://www.projecthoneypot.org/list_of_ips.php?ctry=MU&by=9&rss=1", "ProjectHoneyPot.org", CountryCode.MU);
                    sb.AppendLine(string.Format("= Harvest RSS feed status\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Harvest RSS feed status\t\t : {0}.<br/>", ex.ToString())); }

                try
                {
                    // Total Spam Yield
                    ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromRSS("http://www.projecthoneypot.org/list_of_ips.php?ctry=MU&by=12&rss=1", "ProjectHoneyPot.org", CountryCode.MU);
                    sb.AppendLine(string.Format("= Spam yield RSS feed status\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Spam yield RSS feed status\t\t : {0}.<br/>", ex.ToString())); }

                try
                {
                    // Total Spam
                    ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromRSS("http://www.projecthoneypot.org/list_of_ips.php?ctry=MU&by=15&rss=1", "ProjectHoneyPot.org", CountryCode.MU);
                    sb.AppendLine(string.Format("= Spam RSS feed status\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Spam RSS feed status\t\t : {0}.<br/>", ex.ToString())); }

                try
                {
                    // Total Dictionnary Attack
                    ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromRSS("http://www.projecthoneypot.org/list_of_ips.php?ctry=MU&by=18&rss=1", "ProjectHoneyPot.org", CountryCode.MU);
                    sb.AppendLine(string.Format("= Dictionnary attack RSS feed status\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Dictionnary attack RSS feed status\t\t : {0}.<br/>", ex.ToString())); }

                try
                {
                    // Defaced websites
                    ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromRSS("http://www.zone-h.org/rss/defacements", "www.zone-h.org", CountryCode.UNKNOWN);
                    sb.AppendLine(string.Format("= Defaced RSS feed status\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Defaced RSS feed status\t\t : {0}.<br/>", ex.ToString())); }

                // STATISTICS
                try
                {
                    ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromHTML("http://www.blocklist.de/en/search.html?as=23889", "//div//div//div//table//tr//td", "Blocklist.de", CountryCode.MU);
                    ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromHTML("http://www.blocklist.de/en/search.html?as=30999", "//div//div//div//table//tr//td", "Blocklist.de", CountryCode.MU);
                    sb.AppendLine(string.Format("= BlockList.DE \t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= BlockList.DE \t\t : {0}.<br/>", ex.ToString())); }

                // PHISING
                try
                {
                    ISPStatsConsolidatorService.Singleton.BuildDSLReport("http://www.dslreports.org/phishtrack?xml=1", CountryCode.MU);
                    //  ISPStatsConsolidatorService.Singleton.BuildInfectionStatsFromRSS("http://www.millersmiles.co.uk/millersmiles.xml", "millersmiles.co.uk", CountryCode.MU);
                    sb.AppendLine(string.Format("= DSL Report Phistank : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= DSL Report Phistank \t\t : {0}.<br/>", ex.ToString())); }

                // PHISING
                try
                {
                    ISPStatsConsolidatorService.Singleton.BuildAA419("http://db.aa419.org/fakebankslist.php?x_Status=active&z_Status=LIKE%2C%27%25%2C%25%27", CountryCode.MU);
                    for (int i = 21; i <= 101; i += 20)
                        ISPStatsConsolidatorService.Singleton.BuildAA419(string.Format("http://db.aa419.org/fakebankslist.php?start={0}", i), CountryCode.MU);
                    sb.AppendLine(string.Format("= AA419\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= AA419\t\t : {0}.<br/>", ex.ToString())); }

                // PHISING
                try
                {
                    // temporary disable due to inconsistent data
                    ISPStatsConsolidatorService.Singleton.BUILDCleanMX("http://support.clean-mx.de/clean-mx/xmlphishing.php?", CountryCode.MU, "Phishing", "//entries//entry", false, "phishing.clean-mx.de", "127.{0}.16.{1}");
                    ISPStatsConsolidatorService.Singleton.BUILDCleanMX("http://support.clean-mx.de/clean-mx/xmlviruses.php?", CountryCode.MU, "Malware", "//entries//entry", true, "viruses.clean-mx.de", "127.{0}.17.{1}");

                    sb.AppendLine(string.Format("= CLEANMX \t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= CLEANMX \t\t : {0}.<br/>", ex.ToString())); }

                // DONE
                try
                {
                    ISPStatsConsolidatorService.Singleton.BuildThreatMonitorReport("http://www.threatexpert.com/", "http://www.threatexpert.com/reports.aspx?page={0}&sl=1", CountryCode.MU, 1, 10);
                    sb.AppendLine(string.Format("= ThreatExpert \t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= ThreatExpert \t\t : {0}.<br/>", ex.ToString())); }

                //// DONE
                //try
                //{
                //    // http://labs.snort.org/iplists/
                //    for (int i = 0; i < 10; i++)
                //    {
                //        // http://labs.snort.org/iplists/dnslist-2011-01-01
                //        ISPStatsConsolidatorService.Singleton.BuildSourceFireVRT("http://labs.snort.org/iplists/", CountryCode.MU, string.Format("dnslist-{0}", DateTime.Now.AddDays(-1 * i).ToString("yyyy-MM-dd")));
                //    }
                //    sb.AppendLine(string.Format("= Labs.snort \t\t : {0}.<br/>", "OK"));
                //}
                //catch (Exception ex) { sb.AppendLine(string.Format("= Labs.snort \t\t : {0}.<br/>", ex.ToString())); }

                // DONE
                try
                {
                    ISPStatsConsolidatorService.Singleton.BuildVXVault("http://vxvault.siri-urz.net/ViriList.php?s=0&m=9935", CountryCode.UNKNOWN);
                    sb.AppendLine(string.Format("= VXVault \t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= VXVault \t\t : {0}.<br/>", ex.ToString())); }

                // DONE
                try
                {
                    ISPStatsConsolidatorService.Singleton.BuildThreatCenterReport("http://threatcenter.crdf.fr/", "http://threatcenter.crdf.fr/?p={0}", CountryCode.UNKNOWN, 1, 100);
                    sb.AppendLine(string.Format("= ThreatCenter \t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= ThreatCenter \t\t : {0}.<br/>", ex.ToString())); }

                // DONE
                try
                {
                    Console.WriteLine("Resolving IPs ...");
                    ISPStatsConsolidatorService.Singleton.CalculateIPs(CountryCode.MU);
                    ISPStatsConsolidatorService.Singleton.CalculateIPs(CountryCode.UNKNOWN);
                    sb.AppendLine(string.Format("= IP lookup for empty IPs\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= IP lookup for empty IPs\t\t : {0}.<br/>", ex.ToString())); }

                // DONE
                try
                {
                    Console.WriteLine("Resolving ASNs ...");
                    ISPStatsConsolidatorService.Singleton.CalculateASN(CountryCode.MU);
                    ISPStatsConsolidatorService.Singleton.CalculateASN(CountryCode.UNKNOWN);
                    sb.AppendLine(string.Format("= ASN lookup for known IPs\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= ASN lookup for known IPs\t\t : {0}.<br/>", ex.ToString())); }
            }

            #region " White List "

            if (performWhitelistCleanup)
            {
                try
                {
                    Console.WriteLine("Performing whitelist cleanup ...");
                    ISPStatsConsolidatorService.Singleton.ConsolidateWhitelistDatabase("MU");
                    sb.AppendLine(string.Format("= Database whitelist cleanup\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Database whitelist cleanup\t\t : {0}.<br/>", ex.ToString())); }
            }

            #endregion

            #region " HP-Host "

            if (performDatabaseCleanup)
            {
                try
                {
                    Console.WriteLine("Performing database cleanup ...");
                    ISPStatsConsolidatorService.Singleton.ConsolidateMalwareDatabase(true, true);
                    sb.AppendLine(string.Format("= Database malware cleanup\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Database malware cleanup\t\t : {0}.<br/>", ex.ToString())); }
            }

            #endregion

            if (updateBlockedDomainIPs)
            {
                try
                {
                    Console.WriteLine("Filling blocked domains IPs ...");
                    ISPStatsConsolidatorService.Singleton.UpdateLoggedBlockedDomainsIPs();
                    sb.AppendLine(string.Format("= Filling blocked domains IPs \t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Filling blocked domains IPs  : {0}.<br/>", ex.ToString())); }
            }

            #region " Reporting Statistics "

            if (buildStatistics)
            {
                try
                {
                    Console.WriteLine("Filling blocked domains IPs ...");
                    for (int i = 0; i <= 3; i++)
                    {
                        Console.WriteLine("Day is " + (-1 * i));
                        ISPStatsConsolidatorService.Singleton.CalculateDailyStats(DateTime.Today.AddDays((-1 * i)), false, "ans_ip");
                    }
                    sb.AppendLine(string.Format("= Stats generation status\t\t : {0}.<br/>", "OK"));
                }
                catch (Exception ex) { sb.AppendLine(string.Format("= Stats generation status\t\t : {0}.<br/>", ex.ToString())); }
            }

            #endregion

            #region Email

            try
            {
                sb.AppendLine("=========================================<br/>");
                sb.AppendLine(String.Format("= RSS parsing completed at : {0}.<br/>", DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss tt")));
                sb.AppendLine("=========================================</font>");

                Console.WriteLine("Sending status report ...");
                MailService.SendMail(ConfigurationSettings.AppSettings["EmailWebMaster"], ConfigurationSettings.AppSettings["RSSClient"], String.Format("RSS Stats for {0}", DateTime.Now.ToString("yyyy-MM-dd")), sb.ToString());
            }
            catch (Exception ex) { Console.WriteLine("Mail send error " + ex.ToString()); }

            #endregion

            Console.WriteLine("\nOperation success ...");
        }
    }
}
