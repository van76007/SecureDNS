namespace Core.Membership
{
    using System;
    using System.Collections.Generic;
    using System.Data;
    using System.Data.Common;
    using Microsoft.Practices.EnterpriseLibrary.Data;



    internal static class AccountData
    {
        private const string DNSConnectionString = "DNSConnectionString";



        private static Account CreateAccountFromReader(IDataRecord reader)
        {
            AccountStatus accountStatus = statuses.ContainsKey(reader["status"].ToString()) ? statuses[reader["status"].ToString()] : AccountStatus.LOCKED;
            return new Account(reader["email"].ToString(), reader["password"].ToString(), DateTime.Parse(reader["dateCreated"].ToString()), accountStatus);
        }



        private static readonly Dictionary<string, AccountStatus> statuses = new Dictionary<string, AccountStatus>
                                                                                 {
                                                                                     {"LOCKED", AccountStatus.LOCKED},
                                                                                     {"ACTIVE", AccountStatus.ACTIVE},
                                                                                     {"INACTIVE", AccountStatus.INACTIVE}
                                                                                 };



        public static Account GetAccountByEmail(string email)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"SELECT * FROM users WHERE email = @email";

            DbCommand dbCommand = db.GetSqlStringCommand(query);
            db.AddInParameter(dbCommand, "@email", DbType.String, email);

            Account account = null;

            using (IDataReader reader = db.ExecuteReader(dbCommand))
            {
                if (reader.Read())
                {
                    account = CreateAccountFromReader(reader);
                }
            }

            return account;
        }



        public static bool Save(string email, string password, string status)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"INSERT INTO users (email, password, status)
                                    VALUES (@email, @password, @status)";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@email", DbType.String, email);
            db.AddInParameter(dbCommand, "@password", DbType.String, password);
            db.AddInParameter(dbCommand, "@status", DbType.String, status);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }



        public static bool ChangePassword(string email, string password)
        {
            throw new NotImplementedException();
        }



        public static bool ChangeStatus(string email, string status)
        {
            Database db = DatabaseFactory.CreateDatabase(DNSConnectionString);

            const string query = @"UPDATE users SET status = @status WHERE email = @email";

            DbCommand dbCommand = db.GetSqlStringCommand(query);

            db.AddInParameter(dbCommand, "@email", DbType.String, email);
            db.AddInParameter(dbCommand, "@status", DbType.String, status);

            return db.ExecuteNonQuery(dbCommand) > 0;
        }
    }
}