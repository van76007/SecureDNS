#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

int main(int argc, char *argv[]){

/*	int res;
	char line[512];
	res = open("Read.txt", O_WRONLY);
	//for(int i = 0; i < 5; i++){
		std::string sent = "192.168.230.78 topupdaters.ru 12.1.5.0 multi.surbl.org";
		sent += "\n";
		write(res,sent.c_str(),strlen(sent.c_str())); 
	close(res);
	return 1;
*/

int res;
	//char line[512];
	res = open("Read.txt", O_WRONLY );
	std::string sent = "compleDom";
	/*sent += "(";
	sent += domain;
	sent += "(";
	sent += client_ip;
	sent += "(";
	sent += response;
	sent += "(";
	sent += zoneName;
*/	//std::string sent((i));
	sent += "\n";
	write(res,sent.c_str(),strlen(sent.c_str())); 
	close(res);
	return 1;

}
