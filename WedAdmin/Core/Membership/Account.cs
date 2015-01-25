namespace Core.Membership
{
    using System;



    public enum AccountStatus
    {
        LOCKED,
        ACTIVE,
        INACTIVE
    }

    public class Account
    {
        public string Email { get; private set; }
        private readonly string password;
        public DateTime CreatedOn { get; private set; }
        public AccountStatus Status { get; set; }



        public Account(string email, string pwd)
        {
            Email = email;
            password = pwd;
            CreatedOn = DateTime.Now;
            Status = AccountStatus.ACTIVE;
        }



        public Account(string email, string pwd, DateTime createdOn, AccountStatus status)
            : this(email, pwd)
        {
            CreatedOn = createdOn;
            Status = status;
        }



        internal bool HasPassword(string encryptedPassword)
        {
            return password == encryptedPassword;
        }
    }
}