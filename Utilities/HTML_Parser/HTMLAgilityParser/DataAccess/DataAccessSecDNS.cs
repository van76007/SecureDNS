using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Practices.EnterpriseLibrary.Data;
using System.Data.Common;
using System.Data;

namespace HTMLAgilityParser.DataAccess
{
    internal class DataAccessSecDNS
    {
        internal static int DeleteDomain(string domain, string countryCode)
        {
            Database db = DatabaseFactory.CreateDatabase();

            const string query = @" DELETE  FROM whitelist 
                                   
                                    WHERE   (hostname = ?domain OR ?domain = '')
                                    AND     (country_code = ?country_code OR ?country_code = '')";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "?domain", DbType.String, domain);
            db.AddInParameter(dbCommand, "?country_code", DbType.String, countryCode);

            return db.ExecuteNonQuery(dbCommand);
        }

        internal static int SaveDomain(string domain, string countryCode, int rank, string ip, string notes, string source)
        {
            Database db = DatabaseFactory.CreateDatabase();

            const string query = @" INSERT INTO whitelist 
                                    (
                                        hostname, 
                                        country_code, 
                                        ip,
                                        rank,
                                        notes, 
                                        source
                                    )
                                    VALUES 
                                    (
                                        ?domain,
                                        ?country_code,
                                        ?ip,
                                        ?rank, 
                                        ?notes,
                                        ?source
                                    )";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "?domain", DbType.String, domain);
            db.AddInParameter(dbCommand, "?country_code", DbType.String, countryCode);
            db.AddInParameter(dbCommand, "?notes", DbType.String, notes);
            db.AddInParameter(dbCommand, "?source", DbType.String, source);
            db.AddInParameter(dbCommand, "?ip", DbType.String, ip);
            db.AddInParameter(dbCommand, "?rank", DbType.Int32, rank);

            return db.ExecuteNonQuery(dbCommand);
        }
    }
}
