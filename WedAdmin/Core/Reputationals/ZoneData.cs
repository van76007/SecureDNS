namespace Core.Reputationals
{
    using System.Collections.Generic;
    using System.Data;
    using System.Data.Common;
    using Categories;
    using Microsoft.Practices.EnterpriseLibrary.Data;



    internal static class ZoneData
    {
        private const string DNSConnectionString = "DNSConnectionString";

        private static readonly Dictionary<string, ZoneType> types = new Dictionary<string, ZoneType>
                                                                         {
                                                                             {"REVERSE", ZoneType.REVERSE},
                                                                             {"DOMAIN", ZoneType.DOMAIN},
                                                                             {"BOTH", ZoneType.BOTH}
                                                                         };



        private static Zone CreateZoneFromReader(IDataRecord reader)
        {
            ZoneType type = types.ContainsKey(reader["type"].ToString()) ? types[reader["type"].ToString()] : ZoneType.DOMAIN;
            return new Zone(reader["name"].ToString(), type);
        }



        public static bool Save(string zoneName, string type)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT INTO zones (name, type) 
                                    VALUES (@name, @type) 
                                    on DUPLICATE KEY
                                    UPDATE 
                                    name = @name, 
                                    type = @type";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@name", DbType.String, zoneName);
            db.AddInParameter(dbCommand, "@type", DbType.String, type);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static bool DeleteByName(string zoneName)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"DELETE FROM zones WHERE name = @name";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@name", DbType.String, zoneName);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static Zone GetByName(string zoneName)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT z.*, r.* 
                                    FROM zones z 
                                    LEFT OUTER JOIN responses r 
                                    ON r.zone_name = z.name 
                                    WHERE z.name = @zoneName";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@zoneName", DbType.String, zoneName);

            Zone zone = null;

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    if (zone == null) zone = CreateZoneFromReader(reader);

                    if (!string.IsNullOrEmpty(reader["response"].ToString()))
                        zone.AddResponse(reader["response"].ToString(), new Category {Name = reader["category_name"].ToString()});
                }
            }

            return zone;
        }



        public static bool SaveResponse(string zoneName, string categoryName, string response)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT INTO responses (zone_name, category_name, response) 
                                    VALUES (@zoneName, @categoryName, @response)
                                    ON DUPLICATE KEY 
                                    UPDATE 
                                    category_name = @categoryName";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@zoneName", DbType.String, zoneName);
            db.AddInParameter(dbCommand, "@categoryName", DbType.String, categoryName);
            db.AddInParameter(dbCommand, "@response", DbType.String, response);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static bool DeleteResponse(string zoneName, string responseIP)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"DELETE FROM responses WHERE response = @response AND zone_name = @zoneName";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@zoneName", DbType.String, zoneName);
            db.AddInParameter(dbCommand, "@response", DbType.String, responseIP);


            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static ZoneResponse GetResponse(string zoneName, string responseIP)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);


            const string query = @"SELECT * FROM responses WHERE zone_name = @zoneName AND response = @response";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@zoneName", DbType.String, zoneName);
            db.AddInParameter(dbCommand, "@response", DbType.String, responseIP);

            ZoneResponse response = null;

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                if (reader.Read())
                {
                    response = new ZoneResponse(responseIP, new Category(reader["category_name"].ToString(), "", ""));
                }
            }

            return response;
        }



        public static int SaveResponses(Zone zone)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            string query = @"INSERT INTO responses (zone_name, category_name, response) VALUES ";

            int rowsModified = 0;

            if (zone.Responses.Count > 0)
            {
                for (int i = 0; i < zone.Responses.Count; i++)
                {
                    if (i > 0)
                    {
                        query += @", ";
                    }

                    query += @"(@zoneName, @categoryName" + i + @", @response" + i + @")";
                }

                DbCommand dbCommand = db.GetSqlStringCommand(query);
                db.AddInParameter(dbCommand, "@zoneName", DbType.String, zone.Name);

                for (int i = 0; i < zone.Responses.Count; i++)
                {
                    db.AddInParameter(dbCommand, "@response" + i, DbType.String, zone.Responses[i].ResponseIP);
                    db.AddInParameter(dbCommand, "@categoryName" + i, DbType.String, zone.Responses[i].Category.Name);
                }

                rowsModified = db.ExecuteNonQuery(dbCommand);
            }

            return rowsModified;
        }



        public static List<Zone> GetAll()
        {
            List<Zone> zones = new List<Zone>();

            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT * FROM zones";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    zones.Add(CreateZoneFromReader(reader));
                }
            }

            return zones;
        }
    }
}