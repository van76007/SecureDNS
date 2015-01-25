namespace Core.Domains
{
    using System;
    using System.Collections.Generic;
    using System.Data;
    using System.Data.Common;
    using Microsoft.Practices.EnterpriseLibrary.Data;



    internal static class DomainData
    {
        private const string DNSConnectionString = "DNSConnectionString";


        private static readonly Dictionary<string, string> tables = new Dictionary<string, string>
                                                                        {
                                                                            {"MALWARE", "malware"},
                                                                            {"CAI", "cai"},
                                                                            {"WHITELISTED", "whitelist"},
                                                                        };



        public static bool SaveMalware(string hostname, string category, string source, string description, string targetHost)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT INTO malware (hostname, category, source, description, target_host) 
                                    VALUES (@hostname, @category, @source, @description, @targetHost)";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);
            db.AddInParameter(dbCommand, "@source", DbType.String, source);
            db.AddInParameter(dbCommand, "@category", DbType.String, category);
            db.AddInParameter(dbCommand, "@description", DbType.String, description);
            db.AddInParameter(dbCommand, "@targetHost", DbType.String, targetHost);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static bool SaveCAI(string hostname, string source)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT INTO cai (hostname, source) VALUES (@hostname, @source)";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);
            db.AddInParameter(dbCommand, "@source", DbType.String, source);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static bool SaveWhiteList(string hostname, string countryCode, string notes, string source)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT INTO whitelist (hostname, country_code, notes, source) VALUES (@hostname, @countryCode, @notes, @source)";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);
            db.AddInParameter(dbCommand, "@countryCode", DbType.String, countryCode);
            db.AddInParameter(dbCommand, "@notes", DbType.String, notes);
            db.AddInParameter(dbCommand, "@source", DbType.String, source);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static List<Domain> GetByName(string hostname, DomainType type)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            string query = @"SELECT * FROM " + tables[type.ToString()] + @" WHERE hostname = @hostname";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);

            List<Domain> domains = new List<Domain>();

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    Domain domain = new Domain(reader["hostname"].ToString(), type) { Source = reader["source"].ToString(), Created = DateTime.Parse(reader["date_created"].ToString())};

                    if (!string.IsNullOrEmpty(reader["modified"].ToString()))
                    {
                        domain.Modified = DateTime.Parse(reader["modified"].ToString());
                    }

                    if (type == DomainType.MALWARE)
                    {
                        domain.Description = reader["description"].ToString();
                        domain.TargetHost = reader["target_host"].ToString();
                        domain.CategoryName = reader["category"].ToString();
                    }

                    domains.Add(domain);
                }
            }

            return domains;
        }



        public static int Purge(DateTime threshHold, DomainType type)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            string query = @"DELETE FROM " + tables[type.ToString()] + @" WHERE modified < @modifiedThreshold";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@dmodifiedThreshold", DbType.DateTime, threshHold);

            return db.ExecuteNonQuery(dbCommand);
        }



        public static bool Delete(string hostname, string source, DomainType type)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            string query = @"DELETE FROM " + tables[type.ToString()] + @" WHERE hostname = @hostname AND source = @source";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);
            db.AddInParameter(dbCommand, "@source", DbType.String, source);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static List<Domain> SearchDomains(string searchCriteria, DomainType type)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            string query = @"SELECT * FROM " + tables[type.ToString()] + @" WHERE hostname LIKE @searchCriteria";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@searchCriteria", DbType.String, @"%" + searchCriteria + @"%");

            List<Domain> domains = new List<Domain>();

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    Domain domain = new Domain(reader["hostname"].ToString(), type) {Source = reader["source"].ToString()};

                    if (!string.IsNullOrEmpty(reader["modified"].ToString()))
                    {
                        domain.Modified = DateTime.Parse(reader["modified"].ToString());
                    }

                    if (type == DomainType.MALWARE)
                    {
                        domain.Created = DateTime.Parse(reader["date_created"].ToString());
                        domain.Description = reader["description"].ToString();
                        domain.TargetHost = reader["target_host"].ToString();
                        domain.CategoryName = reader["category"].ToString();
                    }

                    domains.Add(domain);
                }
            }

            return domains;
        }
    }
}