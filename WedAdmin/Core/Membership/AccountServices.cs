namespace Core.Membership
{
    using System;
    using System.Security.Cryptography;
    using System.Text;



    public class AccountServices
    {
        private AccountServices()
        {
        }



        public static readonly AccountServices singleton = new AccountServices();



        public bool Authenticate(string email, string pwd)
        {
            Account account = AccountData.GetAccountByEmail(email);
            bool authentication = false;

            if (account != null)
            {
                authentication = account.HasPassword(EncryptPassword(pwd));
            }

            return authentication;
        }



        public bool Save(string email, string password, string status)
        {
            return AccountData.Save(email, EncryptPassword(password), status);
        }



        public bool ChangePassword(string email, string password)
        {
            return AccountData.ChangePassword(email, password);
        }



        public bool ChangeStatus(string email, string status)
        {
            return AccountData.ChangeStatus(email, status);
        }



        private static string EncryptPassword(string pwd)
        {
            MD5 md5 = new MD5CryptoServiceProvider();
            byte[] originalBytes = Encoding.Default.GetBytes(pwd);
            byte[] encodedBytes = md5.ComputeHash(originalBytes);

            return BitConverter.ToString(encodedBytes);
        }
    }
}