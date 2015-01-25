#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

#define BLEN 512

int main(){

	int fd, ret_val, count, numread;
        char bufpipe[BLEN];
	std::vector <std::string> SplitOutput;
	//string bufpipe;
     	while(1){
		ret_val = mkfifo("Read.txt", 0666);	
		if (( ret_val == -1) && (errno != EEXIST)) 
		{
		        perror("Error creating named pipe");
		        exit(1);
		}
		fd = open("Read.txt", O_RDONLY);
		if(fd < 1){
			std::cout << "FD failed" << std::endl;
			return 1;	
		}
		numread = read(fd,bufpipe, BLEN);
		bufpipe[numread] = '\0';
		std::cout << bufpipe << std::endl;
		std::cout << "After bufpipe" << std::endl;
		memset(bufpipe,'\0',BLEN +1);
		std::string BP(bufpipe);
		boost::split(SplitOutput, BP, boost::is_any_of("("));
		std::cout << "SplitOutput: "  << SplitOutput.size() << std::endl;
	//	for(int i = 0; i < SplitOutput.size(); i++){
		//	std::cout << i << " " << SplitOutput[0] << std::endl;
	//	}		
	}
}
 
 
