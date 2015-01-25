// string::find
#include <iostream>
#include <string>
#include <fstream>
//#include <strstream>
#include <stdlib.h>


using namespace std;

int main ()
{
//	std::string str ("192.168.230.14=53/");
	std::string str;
	int index = 0;
	
/*	std::ifstream textConfig("config.txt");
	textConfig.getline(line, 2048);
	std::cout << "line: " << line << std::endl;
	textConfig.getline(buffer1, 2048);
*/
	ifstream infile, redundantfile;
	infile.open ("config.txt");
	int numberlines;
 std::string* IPaddrPtr;
	//calculate the number IP addresses in the config file
	while(redundantfile.get() != EOF){
		getline(redundantfile, str, '/');
		numberlines++;		
	}
	std::cout << "Number: " << numberlines << std::endl;
IPaddrPtr = new string[4];
        while(infile.get() != EOF) // To get you all the lines.
        {
	        getline(infile,str, '\n'); // Saves the line in STRING.
	       // std::cout << "line: " << str << std::endl; // Prints our STRING

		string str2 ("=");
		std::string IPAddress, PortNumber;
		size_t IPFound, PortFound;

		// different member versions of find in the same order as above:
		IPFound=str.find(str2);
		if (IPFound!=string::npos)
			cout << "first '=' found at: " << int(IPFound) << endl;	
	
		PortFound=str.find("\\");
		if(PortFound == IPFound){
			IPFound = IPFound - 1;		
		}
		
		IPAddress = str.substr(0,int(IPFound));
		std::cout <<  "IP: " << IPAddress << IPFound << std::endl;

		std::cout << "PF: " << int(PortFound) << std::endl;
		if(PortFound!=string::npos)
			std::cout << "Port: "  << PortFound << std::endl;
		//std::cout << "Length: " <<  int(PortFound) int(IPFound) - 1 << std::endl;
		PortNumber = str.substr(int(IPFound)+1, int(PortFound) - int(IPFound) - 1);
		std::cout <<  "PortNumber: " << PortNumber << std::endl;
			
			
		
		std::cout << "fds" << std::endl;
		if (IPaddrPtr==NULL) exit (1);
		std::cout << "index: " << index << std::endl;
		IPaddrPtr[index] = PortNumber;
	//	std::cout << "You have entered: " << std::endl;
		
		index++;
		
	 }
	std::cout << "Out" << std::endl;
	for (int n=0;n<3;n++) {
		std::cout << "Content is: "  << IPaddrPtr[n] << std::endl;
	}
	
	delete [] IPaddrPtr;

	infile.close();	

	return 0;
}
