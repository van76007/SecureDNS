namespace Core.Categories
{
    using System.Collections.Generic;
    using System.Data;
    using System.Data.Common;
    using Microsoft.Practices.EnterpriseLibrary.Data;



    internal static class CategoryData
    {
        private const string DNSConnectionString = "DNSConnectionString";



        private static Category GetCategoryFromReader(IDataRecord reader)
        {
            return new Category(reader["name"].ToString(), reader["unifiedresponse"].ToString(), reader["description"].ToString());
        }



        public static bool Save(string name, string unifiedResponse, string description)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT INTO categories (name, unifiedresponse, description) 
                                    VALUES (@name, @unifiedResponse, @description) 
                                    on DUPLICATE KEY 
                                    UPDATE name = @name, unifiedresponse = @unifiedResponse, description = @description";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@name", DbType.String, name);
            db.AddInParameter(dbCommand, "@description", DbType.String, description);
            db.AddInParameter(dbCommand, "@unifiedResponse", DbType.String, unifiedResponse);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static Category GetByName(string name)
        {
            Category category = null;

            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT * FROM categories WHERE name = @name";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@name", DbType.String, name);

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                if (reader.Read())
                {
                    category = GetCategoryFromReader(reader);
                }
            }

            return category;
        }



        public static List<Category> GetAll()
        {
            List<Category> categories = new List<Category>();

            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT * FROM categories";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    categories.Add(GetCategoryFromReader(reader));
                }
            }

            return categories;
        }



        public static List<Category> GetRBLsCategoriesByHostname(string hostname)
        {
            List<Category> categories = new List<Category>();

            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT c.*, r.zone_name
									FROM blockeddomainstats b
									INNER JOIN responses r 
									ON r.response = b.response 
									AND r.zone_name = b.zone_name
									INNER JOIN categories c 
									ON c.name = r.category_name
									WHERE b.blockeddomain = @hostname 
									GROUP BY c.name";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    Category cat = GetCategoryFromReader(reader);
                    cat.Notes = reader["zone_name"].ToString();
                    categories.Add(cat);
                }
            }

            return categories;
        }



        public static List<Category> GetMalwareCategoriesByHostname(string hostname)
        {
            List<Category> categories = new List<Category>();

            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT c.*, m.description AS notes, m.source
									FROM malware m 
									INNER JOIN blockeddomainstats b 
									ON b.domain = m.hostname 
									INNER JOIN categories c 
									ON c.unifiedresponse = b.response 
									WHERE b.blockeddomain = @hostname 
                                    AND b.zone_name = ''
									GROUP BY c.name";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@hostname", DbType.String, hostname);

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                while (reader.Read())
                {
                    Category cat = GetCategoryFromReader(reader);
                    cat.Notes = string.Format("{0} - {1}", reader["notes"], reader["source"]);
                    categories.Add(cat);
                }
            }

            return categories;
        }



        public static bool DeleteByName(string name)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"DELETE FROM categories WHERE name = @name";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@name", DbType.String, name);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }
    }
}