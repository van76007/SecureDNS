namespace Core.BlockedIps
{
    using System.Collections.Generic;



    public class BlockedIPServices
    {
        private BlockedIPServices()
        {
        }



        public static readonly BlockedIPServices singleton = new BlockedIPServices();



        public bool Save(string ip, string category, string reason, string source)
        {
            return BlockedIPData.Save(ip, category, reason, source);
        }



        public bool Delete(string ip)
        {
            return BlockedIPData.Delete(ip);
        }



        public List<BlockedIp> CheckIfExists(string[] ips)
        {
            return BlockedIPData.Exists(ips);
        }



        public List<BlockedIp> GetBlockingHistory(string hostname)
        {
            return BlockedIPData.GetBlockingIPsHistory(hostname);
        }



        public List<BlockedIp> GetAll()
        {
            return BlockedIPData.GetAll();
        }
    }
}