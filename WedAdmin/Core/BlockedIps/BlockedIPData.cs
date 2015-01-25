namespace Core.BlockedIps
{
    using System.Collections.Generic;
    using System.Data;
    using System.Data.Common;
    using System.Linq;
    using Microsoft.Practices.EnterpriseLibrary.Data;



    internal class BlockedIPData
    {
        private const string DNSConnectionString = "DNSConnectionString";



        public static bool Save(string ip, string category, string reason, string source)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT IGNORE INTO blocked_ips (ip, Category, Reason, source) VALUES (@ip, @category, @reason, @source)";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@ip", DbType.String, ip);
            db.AddInParameter(dbCommand, "@category", DbType.String, category);
            db.AddInParameter(dbCommand, "@reason", DbType.String, reason);
            db.AddInParameter(dbCommand, "@source", DbType.String, source);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static bool Delete(string ip)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"DELETE FROM blocked_ips WHERE ip = @ip";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@ip", DbType.String, ip);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static List<BlockedIp> Exists(string[] ips)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT * FROM blocked_ips WHERE ip IN({0})";

            List<BlockedIp> blockedIps = new List<BlockedIp>();

            string[] paramNames = ips.Select((s, i) => "@tag" + i.ToString()).ToArray();

            string inClause = string.Join(",", paramNames);

            using (DbCommand dbCommand = db.GetSqlStringCommand(string.Format(query, inClause)))
            {
                for (int i = 0; i < paramNames.Length; i++)
                {
                    db.AddInParameter(dbCommand, paramNames[i], DbType.String, ips[i]);
                }

                using (IDataReader reader = db.ExecuteReader(dbCommand))
                {
                    while (reader.Read())
                    {
                        blockedIps.Add(new BlockedIp
                                           {
                                               Category = reader["Category"].ToString(),
                                               IP = reader["ip"].ToString(),
                                               Reason = reader["Reason"].ToString(),
                                               Source = reader["source"].ToString()
                                           });
                    }
                }
            }

            return blockedIps;
        }



        public static List<BlockedIp> GetBlockingIPsHistory(string hostname)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            List<BlockedIp> blockedIps = new List<BlockedIp>();

            const string query = @"SELECT b.* FROM blockeddomainstats s 
                                    INNER JOIN blocked_ips b 
                                    ON b.ip = s.domainip 
                                    WHERE s.blockeddomain = @hostname AND b.ip != ''";

            using (DbCommand dbCommand = db.GetSqlStringCommand(query))
            {
                db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);

                using (IDataReader reader = db.ExecuteReader(dbCommand))
                {
                    while (reader.Read())
                    {
                        blockedIps.Add(new BlockedIp
                                           {
                                               Category = reader["Category"].ToString(),
                                               IP = reader["ip"].ToString(),
                                               Reason = reader["Reason"].ToString(),
                                               Source = reader["source"].ToString()
                                           });
                    }
                }
            }

            return blockedIps;
        }



        public static List<BlockedIp> GetAll()
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT * FROM blocked_ips";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            List<BlockedIp> ips = new List<BlockedIp>();

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    ips.Add(new BlockedIp()
                                {
                                    Category = reader["Category"].ToString(),
                                    IP = reader["ip"].ToString(),
                                    Reason = reader["Reason"].ToString(),
                                    Source = reader["source"].ToString()
                                });
                }
            }

            return ips;
        }
    }
}