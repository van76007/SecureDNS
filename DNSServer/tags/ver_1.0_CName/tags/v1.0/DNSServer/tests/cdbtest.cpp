#include<iostream>
#include<sstream>
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "openssl/evp.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/bio.h"
#include "openssl/rand.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <vector>
#include <openssl/sha.h>
#include <ctime>
#include <cdb.h>
#include <boost/filesystem.hpp>
#include <mysql/mysql.h>
#include <boost/algorithm/string/trim.hpp>

#define TRIM ((! boost::algorithm::is_print()) || boost::algorithm::is_space())


//split and Build URL
std::vector<std::string> SplitBuildURL(std::string domain, int BL){

 // std::cout << "domain passed : " << domain << std::endl;
  std::vector<int> Delindex;	
  std::vector<std::string> URLlist;	

  std::string::size_type pos = domain.find_first_of(".");
  Delindex.push_back(pos+1);
  std::string Backdomain = domain;
  while(pos != std::string::npos){
    pos = Backdomain.find_first_of(".", pos + 1);
    Delindex.push_back(pos+1);
   // std::cout << "Postion is: " << pos << std::endl;
  }
  URLlist.push_back(domain);
  for(int i = 0; i < (int)Delindex.size() - 2; i++){
    URLlist.push_back(domain.substr( Delindex[i], strlen(domain.c_str()) - Delindex[i] ));
   // std::cout << "URLlist is: " << URLlist[i] << std::endl;
    if(0 == BL) break;
  }
  return URLlist;
}

bool DeleteFile(const std::string dirPath)
{
	const boost::filesystem::path thePath = boost::filesystem::system_complete(dirPath);
	try {
		remove_all(thePath);
	}
	catch (...) {
		//std::cout << dirPath << "-file not deleted" << std::endl;
		return false;
	}
	return true;
}

int RecentFile( std::string f1, std::string f2 ){
	int seconds = time(NULL);			
	struct stat attrib1, attrib2;	
	if( stat(f1.c_str(), &attrib1) < 0 ){
		if ( stat(f2.c_str(), &attrib2)  == 0) return 2; //if cannot stat f1   
		return 0;
	}
	if ( stat(f2.c_str(), &attrib2)  < 0){
		if ( stat(f1.c_str(), &attrib2)  == 0) return 1; //if cannot stat f2
		return 0;	
	}
	int creation_time1 = attrib1.st_ctime;
	int creation_time2 = attrib2.st_ctime;
	if( creation_time1 > creation_time2 ){
		std::cout << "seconds: " << seconds << std::endl;
		std::cout << "creation_time1: " << creation_time1 << std::endl;
		if( (seconds - creation_time1 ) > 60 ) 
			DeleteFile(f2);		
		return 1;
	}else{
		std::cout << "seconds: " << seconds << std::endl;
		std::cout << "creation_time2: " << creation_time2 << std::endl;
		if( (seconds - creation_time2 ) > 60)
			DeleteFile(f1);
		return 2;
	}
	return 0;
}

std::string DetFile(std::string db1, std::string db2){
  std::string db;
  int recent;
  try{
  if( boost::filesystem::exists( db1 ) ){
	if( boost::filesystem::exists(db2) ){
		//look which one is the most recent
		recent = RecentFile(db1, db2);
		if( 0 == recent ) return " ";
		if ( 1 == recent ){
			std::cout << "Latest Creation: " << db1 << std::endl;
			db = db1;		
		}else{
			std::cout << "Latest Creation: " << db2 << std::endl;
			db = db2;
		}			
	}else{
			//Open whitelist.cdb
			db = db1;
	}	
  }else if( boost::filesystem::exists( db2 ) ){
	db = db2;
  }
  }catch(...){return " ";}
 	std::cout << "db is: " << db << std::endl;
  return db;
}



int readDB( std::string dbfile, std::string dupfile, std::string key, int onlyValue, std::vector<std::string> & ListUniResp ){
     int fd;
     int fddup;
     std::cout << "key: " << key << std::endl;
     std::string found = " ";
     if(onlyValue == 2){
        std::string k(boost::algorithm::trim_copy_if( key, TRIM));
	key = k;
     }
     std::vector <std::string> uniList;
     // Open db file
     fd = open(dbfile.c_str(), O_RDONLY);
     if(fd == -1){
	  std::cout << "ERROR" << std::endl;
	  fddup = open(dupfile.c_str(), O_RDONLY);
	  if(fddup == -1){
		std::cout << "Failed to open: " << dupfile <<std::endl;
		return 1;
	 }else{
		fd = fddup;
		std::cout << "Worked out: " << dupfile << std::endl;
	}
	  return 1;
     } else {
	  std::cout << "File fine" << std::endl;
     }
     std::cout << "ValueString" <<  std::endl;
     std::string ValueString; ValueString.reserve(20);

	struct cdb cdb;
	char *val;
	unsigned vpos1, vlen1;

	cdb_init(&cdb, fd); // initialize internal structure 

  std::cout << "cdb find: " << key << "e" << std::endl;

  struct cdb_find cdbf; // structure to hold current find position 
  int err = cdb_findinit(&cdbf, &cdb, key.c_str(), key.size() ); // initialize search of key 
  if(err == 0) {
	perror("findinit No more key");
	cdb_free(&cdb);
	close(fd);
	return 1;
  }
  if(err < 0){
	perror("findinit Error");
	cdb_free(&cdb);
	close(fd);
	
	fd = open(dupfile.c_str(), O_RDONLY);
	if(fd < 0) return 1;
	cdb_init(&cdb, fd);
  	int e = cdb_findinit(&cdbf, &cdb, key.c_str(), key.size() ); 
	if( e < 0){
		perror("findinit Error");
		cdb_free(&cdb);
		close(fd);
		return 1;
	}
  }
 // std::cout << "before while" << onlyValue << std::endl;
  while(cdb_findnext(&cdbf) > 0) {
    std::cout << "OOps found" << std::endl;
    if(onlyValue == 1){
	cdb_free(&cdb);
	close(fd);
	//ListUniResp.push_back("f");
	return 0;
	}
    vpos1 = cdb_datapos(&cdb);
    vlen1 = cdb_datalen(&cdb);
    val =(char*)malloc(vlen1 * sizeof(char*));
    cdb_read(&cdb, val, vlen1, vpos1);
    val[vlen1] = '\0';
    std::string fo(val);
    found = fo;
    ListUniResp.push_back(found);
    std::cout << "val: " << found << std::endl;
    // handle the value 
    free(val);
  }
  cdb_free(&cdb);
  close(fd);
  return 1;
}

int QueryDatabases(std::string bl, std::string completeDomain, std::vector <int> statusParser){

    std::string clientIPAdd("Disabled");
    std::string comDom = completeDomain;
#ifdef DEBUG
	std::cout << "bl: " << bl  << std::endl;	
#endif 
  if(bl.compare(" ")) comDom = bl;
#ifdef DEBUG
  std::cout << "CompleteDomain: " << comDom << std::endl;
#endif 
  int BlockedID;//= mDNSList->GetBlockedID();
  if(statusParser[1] || statusParser[2] ) BlockedID = 2;
  int MALWARE_L = 1;
  int CHILD_L = 3;
  int Status = 100;
  int found = 0;
  if (statusParser[0]) {
   Status = MALWARE_L;
  }
  else if (statusParser[1]){
    Status = MALWARE_L;
  }
  else if(statusParser[2]){
    Status = CHILD_L;
  }
//std::vector<std::string> NewURLs; std::vector<std::string> WWWremoved;
  /**Split the domain and Integrate**/
  std::vector<std::string> NewURLs = SplitBuildURL(completeDomain, 1);
  /****/
  std::vector<std::string> WWWremoved = SplitBuildURL(completeDomain, 0); 
  //determine the recent
  std::string white1 = "whitelist1.cdb"; std::string white2 = "whitelist2.cdb"; std::string dubwhite;
  std::string mal1 = "malware1.cdb"; std::string mal2 = "malware2.cdb"; std::string dubmal; 
  std::string chi1 = "child1.cdb"; std::string chi2 = "child2.cdb"; std::string dubchi;

 // std::string LatWhite = "whitelist1.cdb";
  std::string LatWhite = DetFile(white1, white2);
  if( !LatWhite.compare(white1)){dubwhite = white2; }
  else{dubwhite = white1;}
  std::string LatMal = DetFile( mal1, mal2 );
  if(!LatMal.compare(mal1)){ dubmal = mal2; }
  else{dubmal = mal1;}
  if(!LatMal.compare(" ")) return 0;
  std::string LatChi = DetFile( chi1, chi2 );
  if(!LatChi.compare(chi1)){dubchi = chi2;}
  else{dubchi = chi1;}
  if(!LatChi.compare(" ")) return 0;

  std::vector<std::string> whiteUni;
  std::vector<std::string> malUni;
  std::vector<std::string> chiUni;
  //lookup white list
  //std::cout << "Looking up the whitelist database" << std::endl;
  for(int x = 0; x < (int)NewURLs.size();x++){
    int r = readDB( LatWhite, dubwhite, NewURLs[x], 1, whiteUni );
    if( r == 0 ){
	std::cout << "FOUND in Whitelist" << std::endl;
	return 2;
    }else{ /*std::cout << "Not Found" << std::endl;*/}
  }
  switch( Status ){
  case 0://lookup RBL //Not operational because the RBL database is not yet designed
    break;
  case 1://lookup Malware
 	for(int j = 0; j < (int)NewURLs.size(); j++){
		std::cout << "Within Malware OOPs" << std::endl;
		readDB(LatMal, dubmal, NewURLs[j], 0, malUni);
		std::cout<< "After readdb" << std::endl;
		if(malUni.size() != 0){
			for(int i = 0; i < (int)malUni.size(); i++ ){
				std::cout << "Before Writetolog" << std::endl;
				//writetolog(comDom, NewURLs[j], clientIPAdd, malUni[i], " ");
				malUni.clear();
				std::cout << "After clearing malUni" << std::endl;		
			}
			found = 1;
		}//end if statement
	}//end for loop
	if (found == 1) return 3;
      case 3://lookup Child
	for(int j = 0; j < (int)NewURLs.size(); j++){
		readDB(LatChi, dubchi, NewURLs[j], 0, chiUni);
		if( chiUni.size() != 0 ){
			for(int i = 0; i < (int)chiUni.size(); i++){
				//writetolog(comDom, NewURLs[j], clientIPAdd, chiUni[i], " ");
				malUni.clear();
			}
			found = 1;
		}//end if statement
	}//end for loop
	if (found == 1) return 3;
    break;
  default:
   // std::cout << "No database Parsed. WHY??. This should not happen" << std::endl;
    return 0;
    break;
  }
#ifdef DEBUG
  std::cout << "Domain not found within Database" << std::endl;
#endif
  return 0;	
}//end query databases

int main(int argc, char *argv[]){

 std::string domain(argv[1]);
 std::vector<int> statusParser;
 statusParser.push_back(0);statusParser.push_back(1);statusParser.push_back(0);
 int domainFound = QueryDatabases(" ", domain, statusParser);

}
