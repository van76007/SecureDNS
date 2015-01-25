/**
 * @file
 * @author SNU <snu@csis.dk>
 * @author DCO <dco@csis.dk>	
 * @brief Header file for DNSList
 *
 * @section Description
 * This file contains the definition of DNSList
 * 
 *
 */

#include <stddef.h>
#include <string>
#include <string.h>
#include <mysql/mysql.h>

#ifndef NAMESERVER_DNSLIST_H
#define NAMESERVER_DNSLIST_H

namespace Nameserver {
  class DNSList
  {
  private:
  static DNSList* mpsInstance;
    	
  public:
  DNSList();
 ~DNSList();
  static DNSList* GetInstance();
  void setinte(int integ);
  void setIPNameserver(std::string IPName);
  std::string GetIPNameserver();
  void setPortNameserver(int PN);
  int GetPortNameserver();

  void setDatabaseIP(std::string DBIP);
  std::string GetDatabaseIP();

  void setDatabseUser(std::string DBuser);
  std::string GetDatabseUser();

  void setDatabasePass(std::string DBPass);
  std::string GetDatabasePass();

  void setDatabaseName(std::string DBuser);
  std::string GetDatabaseName();	

  MYSQL* GetMysqlConn();
  void setMySQLConn(MYSQL* conn);

  void setBlockedID(int blo);
  int GetBlockedID();

  void setZoneName(std::string zone);
  std::string GetZoneName();

  void setByteCnt(int bytecnt);
  int GetByteCnt();

  void setResStr(std::string rstr);
  std::string GetResStr();

  void setNS(int ns);
  int GetNS();

  void setBlocked(std::string blockedP);
  std::string GetBlocked();
  }; 
}; //namespace Namserver


#endif // NAMESERVER_DNSLIST_H
