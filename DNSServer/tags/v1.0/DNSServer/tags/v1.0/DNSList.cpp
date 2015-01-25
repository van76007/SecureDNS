#include <DNSList.h>


Nameserver::DNSList* Nameserver::DNSList::mpsInstance = NULL;

Nameserver::DNSList::DNSList()
{
}; 
Nameserver::DNSList::~DNSList() 
{
}; 

// Static
Nameserver::DNSList* Nameserver::DNSList::GetInstance()
{
  if(mpsInstance == NULL) {
    mpsInstance = new Nameserver::DNSList();
  }
  return mpsInstance;
}; //GetInstance

static std::string IPNameServer;
void Nameserver::DNSList::setIPNameserver(std::string IPName){
  IPNameServer = IPName;
};

std::string Nameserver::DNSList::GetIPNameserver(){
   return IPNameServer;
};

static int PortNumber;
void Nameserver::DNSList::setPortNameserver(int PN){
	PortNumber = PN;
};

int Nameserver::DNSList::GetPortNameserver(){
	return PortNumber;
};

static std::string dataIP;
void Nameserver::DNSList::setDatabaseIP(std::string DBIP){
	dataIP = DBIP;
};

std::string Nameserver::DNSList::GetDatabaseIP(){
	return dataIP;
};

static std::string dataUser;
void Nameserver::DNSList::setDatabseUser(std::string DBuser){
	dataUser = DBuser;
};

std::string Nameserver::DNSList::GetDatabseUser(){
	return dataUser;
};

static std::string dataPass;
void Nameserver::DNSList::setDatabasePass(std::string DBPass){
	dataPass = DBPass;
};

std::string Nameserver::DNSList::GetDatabasePass(){
	return dataPass;
};


static std::string dataName;
void Nameserver::DNSList::setDatabaseName(std::string DBuser){
	dataName = DBuser;
};

std::string Nameserver::DNSList::GetDatabaseName(){
	return dataName;
};

static MYSQL* Connection; 
void Nameserver::DNSList::setMySQLConn(MYSQL* conn){
	Connection = conn;
};

MYSQL* Nameserver::DNSList::GetMysqlConn() {
 return Connection; 
};

static int bl;
void Nameserver::DNSList::setBlockedID(int blo){
	bl = blo;
};

int Nameserver::DNSList::GetBlockedID() {
	return bl;
};

static std::string zn;
void Nameserver::DNSList::setZoneName(std::string zone){
	zn = zone;
};

std::string Nameserver::DNSList::GetZoneName(){
	return zn;
};
static int bcnt;
void Nameserver::DNSList::setByteCnt(int bytecnt){
	bcnt = bytecnt;
};

int Nameserver::DNSList::GetByteCnt(){
	return bcnt;
};
static std::string ResStr;
void Nameserver::DNSList::setResStr(std::string rstr){
	ResStr="";
	ResStr = rstr;
};

std::string Nameserver::DNSList::GetResStr(){
	return ResStr;

};

static int NS;
void Nameserver::DNSList::setNS(int ns){
	NS = ns;
};

int Nameserver::DNSList::GetNS(){
	return NS;
};

static std::string blPg;
void Nameserver::DNSList::setBlocked(std::string blockedP){
	blPg = blockedP;
}

std::string Nameserver::DNSList::GetBlocked(){
	return blPg;
}

