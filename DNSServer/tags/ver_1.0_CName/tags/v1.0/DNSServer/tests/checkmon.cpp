#include <iostream>
#include <cdb.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctime>
#include <sstream>
#include <yaml.h>

//using namespace std;

void readDB( std::string dbfile, std::string key ){
     int fd;
     std::cout << "Searching for Domain: " << key << std::endl;
     std::cout << "" << std::endl;
     // Open db file
     fd = open(dbfile.c_str(), O_RDONLY);
     if(fd == -1){
	std::cout << "Error could not open: " << dbfile << std::endl;
	return;
     } else {
	  //std::cout << "OK" << std::endl;
     }
     std::string ValueString; ValueString.reserve(20);

	struct cdb cdb;

	cdb_init(&cdb, fd); // initialize internal structure 
	char *val;
	unsigned vpos1, vlen1;

  struct cdb_find cdbf; // structure to hold current find position 
  int err = cdb_findinit(&cdbf, &cdb, key.c_str(), key.size() ); // initialize search of key 
  if(err == 0) perror("findinit No more key");
  if(err < 0) perror("findinit Error");
  int i = 0;
  while(cdb_findnext(&cdbf) > 0) {
	i = 1;
	vpos1 = cdb_datapos(&cdb);
    	vlen1 = cdb_datalen(&cdb);
    	val =(char*)malloc(vlen1 * sizeof(char*));
    	cdb_read(&cdb, val, vlen1, vpos1);
    	val[vlen1] = '\0';
	std::cout << "Domain " << key << " has been found in " << dbfile << " with unified Response: " << val << std::endl;
    	//std::cout << "val: " << found << std::endl;
    	// handle the value 
    	free(val);
  }
  if( 0 == i ) std::cout << "Could not find domain " << key << " in " << dbfile << std::endl;
  std::cout << "" << std::endl;
  cdb_free(&cdb);
  close(fd);
}
int main(int argc, char* argv[]){
	if(argc > 3 || argc < 3){
		std::cout << "cdb file and/or domain missing" << std::endl;
		return 1;
	} 
	readDB(argv[1], argv[2]);  
	return 1;
}
