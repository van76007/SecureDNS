/**
 * @file
 * @author SNU <lba@csis.dk>
 * @authot DCO <dco@csis.dk>
 * @brief Header file for DNSEntity
 *
 * @section Description
 * This file contains the definition of all the classes that inherit this file
 * 
 *
 */


#include <string>
#include <string.h>
#include <mysql/mysql.h>
#include <vector>

#ifndef NAMESERVER_DNSENTITY_H
#define NAMESERVER_DNSENTITY_H


namespace Nameserver {

  /**
   * @brief DNS Interface
   *
   */
	 
  class DNSEntity
  {
  public:
    DNSEntity() {};
    ~DNSEntity() {};
    MYSQL* GetMysqlConn() 		{ return Connection; };
    int GetPortNameserver() 		{ return PortNameServer; };
    std::string GetIPNameserver() 		{ return IPNameServer; };
    int GetBlockedID() {return bl;};
    std::string GetIPResponse()		{return IPr;};
    std::string GetZoneName() 		{return zn;};
    std::vector<int> GetBlockVector() 	{return bld;};
    std::string GetDatabaseIP() 	{return DBip;};
    std::string GetDatabaseName()	{return DBn;};
    std::string  GetDatabseUser()	{return DBu;};
    std::string  GetDatabasePass()	{return DBp;};
    int GetByteCnt()			{return bcnt;};
    std::string GetResStr()	{return ResStr;};

    void setMySQLConn(MYSQL* conn) {this->Connection = conn;};
    void setPortNameserver(const int PortName ) {this->PortNameServer = PortName;};
    void setIPNameserver(const std::string &IPName ) {this->IPNameServer = IPName;};
    void setBlockedID(int blo){this->bl = blo;};
    void setIPResponse(std::string IPres){this->IPr = IPres;};
    void setZoneName(std::string zone){this->zn = zone;};
    void setBlockVector(int bln){this->bld.push_back(bln); };
    void setDatabaseIP(std::string dbip){this->DBip = dbip;};
    void setDatabaseName(std::string DBname){this->DBn = DBname;};
    void setDatabseUser(std::string DBuser){this->DBu = DBuser;};
    void setDatabasePass(std::string DBpass){this->DBp = DBpass;};
    void setByteCnt(int bytecnt){this->bcnt = bytecnt;};
    void setResStr(std::string rstr){this->ResStr="";this->ResStr = rstr;};
    void clearBlockVector(){ bld.clear();};

  protected:
    static std::string logFile;
    MYSQL* Connection;
    int PortNameServer;
    std::string IPNameServer;
    int bl;
    std::string IPr;
    std::string zn;
    std::vector<int> bld;
    std::string ipr;
    std::string DBip, DBn, DBu, DBp;
    int bcnt;
    std::string ResStr;
 
  }; //DNS_interface
}; //namespace Nameserver

#endif // NAMESERVER_DNSENTITY_H
