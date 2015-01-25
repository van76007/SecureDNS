using System;
using System.Collections.Generic;
using System.Text;
using System.Data.Common;
using Microsoft.Practices.EnterpriseLibrary.Data;
using System.Data;

namespace HTMLAgilityParser.DataAccess
{
    public static class DataAccessHtmlParser
    {
        #region Database Methods

        internal static int AddMalwareRecord(string hostname, string unified_response, string category, string description, string targethost, string sorce)
        {
            int retCode = -1;
            string query = @"Insert     
                             into       malware(hostname, unified_response, category, description, target_host, source)
                             values     (@hostname, @unified_response, @category,@description,@targethost,@source)
                             on         duplicate key update modified = CURRENT_TIMESTAMP, unified_response = @unified_response, description = @description";

            Database db = DatabaseFactory.CreateDatabase();

            using (DbCommand dbCommand = db.GetSqlStringCommand(query))
            {
                dbCommand.Parameters.Clear();
                db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);
                db.AddInParameter(dbCommand, "@category", DbType.String, category);
                db.AddInParameter(dbCommand, "@description", DbType.String, description);
                db.AddInParameter(dbCommand, "@targethost", DbType.String, targethost);
                db.AddInParameter(dbCommand, "@source", DbType.String, sorce);
                db.AddInParameter(dbCommand, "@unified_response", DbType.String, unified_response);

                if (!unified_response.Equals(string.Empty))
                    retCode = db.ExecuteNonQuery(dbCommand);
            }

            return retCode;
        }

        internal static int AddMalwareRecord(string hostname, string unified_response, string category, string source, string description)
        {
            return AddMalwareRecord(hostname, unified_response, category, description, "", source);
        }

        internal static int AddBlockedIPRecord(string ip, string unified_response, string category, string source, string reason)
        {
            int retCode = -1;
            Database db = DatabaseFactory.CreateDatabase();

            const string query = @" INSERT INTO blocked_ips(ip, unified_response, category, source, reason)                                  
                                    VALUES (@ip, @unified_response, @category, @source, @reason)
                                    on     duplicate key update modified = CURRENT_TIMESTAMP";

            using (DbCommand dbCommand = db.GetSqlStringCommand(query))
            {
                dbCommand.Parameters.Clear();
                db.AddInParameter(dbCommand, "@ip", DbType.String, ip);
                db.AddInParameter(dbCommand, "@category", DbType.String, category);
                db.AddInParameter(dbCommand, "@source", DbType.String, source);
                db.AddInParameter(dbCommand, "@reason", DbType.String, reason);
                db.AddInParameter(dbCommand, "@unified_response", DbType.String, unified_response);

                if (!unified_response.Equals(string.Empty))
                    retCode = db.ExecuteNonQuery(dbCommand);
            }

            return retCode;
        }

        internal static int DeleteMalwareRecord(string hostname)
        {
            Database db = DatabaseFactory.CreateDatabase();

            const string query = @" DELETE  FROM malware                                    
                                    WHERE   hostname = @hostname";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);

            return db.ExecuteNonQuery(dbCommand);
        }

        internal static int DeleteBlockedIPRecord(string ip)
        {
            Database db = DatabaseFactory.CreateDatabase();

            const string query = @" DELETE  FROM blocked_ips                                    
                                    WHERE   ip = @ip";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@ip", DbType.String, ip);

            return db.ExecuteNonQuery(dbCommand);
        }

        #endregion

        #region Exclusions

        internal static int AddExclusion(string hostname, string domain, string regex_pattern, string category, string source)
        {
            string query = @"Insert     Ignore 
                             into       exclusions(hostname, domain, regex_pattern, category, source)
                             values     (?hostname, ?domain, ?regex_pattern, ?category, ?source)";

            Database db = DatabaseFactory.CreateDatabase();
            DbCommand dbCommand = db.GetSqlStringCommand(query);

            dbCommand.Parameters.Clear();
            db.AddInParameter(dbCommand, "?hostname", DbType.String, hostname);
            db.AddInParameter(dbCommand, "?domain", DbType.String, domain);
            db.AddInParameter(dbCommand, "?regex_pattern", DbType.String, regex_pattern);
            db.AddInParameter(dbCommand, "?category", DbType.String, category);
            db.AddInParameter(dbCommand, "?source", DbType.String, source);

            return db.ExecuteNonQuery(dbCommand);
        }

        internal static DataSet GetExclusions(string hostname, string domain, string category)
        {
            DataSet ds = new DataSet();

            string query = @"   select  hostname, IFNULL(regex_pattern, '^dummy_pattern$')
                                from    exclusions
                                where   (hostname = ?hostname)
                                        or (domain = ?domain)";

            Database db = DatabaseFactory.CreateDatabase();

            using (DbCommand dbCommand = db.GetSqlStringCommand(query))
            {
                db.AddInParameter(dbCommand, "?hostname", DbType.String, hostname);
                db.AddInParameter(dbCommand, "?domain", DbType.String, domain);

                db.LoadDataSet(dbCommand, ds, "Exclusions");
            }

            return ds;
        }

        #endregion
    }
}
