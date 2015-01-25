/*
* How to compile: g++ -o test_overwrite test_overwrite.c
*/

#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

int main(void)
{
    ofstream myfile;
	
	for (int i=0; i<20; i++)
	{
	    std::cout << "i is: " << i << std::endl;
		
		myfile.open ("/usr/home/dvv/foo");
		
		//Read_Log_Content();
		if (i%2 == 0) {
		    // if (log_content == "F1 is in use")
			// 	Dump_CDB_To_F2
			//  Update log_content
			myfile << "F2 is in use";
			std::cout << "File content is: " << "F2 is in use at step " << i << std::endl;
		} else {
		    // if (log_content == "F2 is in use")
			// 	Dump_CDB_To_F1
			//  Update log_content
			myfile << "F1 is in use";
			std::cout << "File content is: " << "F1 is in use at step " << i << std::endl;
		}
		myfile.close();
		
		sleep(10); // sleep 10s
	} 
    
    return 0;
}
