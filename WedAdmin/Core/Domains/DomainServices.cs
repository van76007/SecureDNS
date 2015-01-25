namespace Core.Domains
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net;
    using System.Threading;
    using BlockedIps;


    public class DomainServices
    {
        public static readonly DomainServices singleton = new DomainServices();



        private DomainServices()
        {
        }



        public bool SaveMalware(string hostname, string category, string source, string description, string targetHost)
        {
            return DomainData.SaveMalware(hostname, category, source, description, targetHost);
        }



        public Dictionary<DomainType, List<Domain>> GetSearchResult(string searchString)
        {
            Dictionary<DomainType, List<Domain>> domains = new Dictionary<DomainType, List<Domain>>();

            foreach (DomainType t in Enum.GetValues(typeof (DomainType)))
            {
                DomainType type = t;

                //new Thread(delegate() { domains[type] = DomainData.SearchDomains(searchString, type); }).Start();

                domains[type] = DomainData.SearchDomains(searchString, type);
            }

            //while (domains.Keys.Count < Enum.GetNames(typeof (DomainType)).Length)
            //{
            //    Thread.Sleep(10);
            //}

            return domains;
        }


        public IPHostEntry GetHostEntryInfo(string searchString)
        {
            return Dns.GetHostEntry(searchString);
        }



        public Dictionary<string, BlockedIp> GetBlockedResult(string hostname)
        {
            Dictionary<string, BlockedIp> blockedResult = new Dictionary<string, BlockedIp>();

            string[] addressList = Dns.GetHostAddresses(hostname).Select(x => x.ToString()).ToArray();

            if (addressList.Length > 0)
            {
                foreach (var s in addressList)
                {
                    blockedResult[s] = null;
                }

                foreach (var s in BlockedIPServices.singleton.CheckIfExists(addressList))
                {
                    blockedResult[s.IP] = s;
                }
            }

            return blockedResult;
        }



        public Dictionary<string, BlockedIp> GetBlockedResult(IPAddress[] ipAddresses)
        {
            Dictionary<string, BlockedIp> blockedResult = new Dictionary<string, BlockedIp>();

            string[] addressList = ipAddresses.Select(x => x.ToString()).ToArray();

            if (addressList.Length > 0)
            {
                foreach (var s in addressList)
                {
                    blockedResult[s] = null;
                }

                foreach (var s in BlockedIPServices.singleton.CheckIfExists(addressList))
                {
                    blockedResult[s.IP] = s;
                }
            }

            return blockedResult;
        }


        public bool SaveCAI(string hostname, string source)
        {
            return DomainData.SaveCAI(hostname, source);
        }



        public bool SaveWhitelist(string hostname, string countryCode, string notes, string source)
        {
            return DomainData.SaveWhiteList(hostname, countryCode, notes, source);
        }



        public List<Domain> GetByName(string hostname, DomainType type)
        {
            return DomainData.GetByName(hostname, type);
        }



        public int PurgeTable(DateTime threshold, DomainType type)
        {
            return DomainData.Purge(threshold, type);
        }



        public bool Delete(string hostname, string source, DomainType type)
        {
            return DomainData.Delete(hostname, source, type);
        }
    }
}