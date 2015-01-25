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
	
	for (int i=0; i<10; i++)
	{
	    std::cout << "i is: " << i << std::endl;
		
		myfile.open ("/usr/home/dvv/foo");
		if (i%2 == 0) {
			myfile << "F2 is in use at step " << i;
			std::cout << "File content is: " << "F2 is in use at step " << i << std::endl;
		} else {
			myfile << "F1 is in use at step " << i;
			std::cout << "File content is: " << "F1 is in use at step " << i << std::endl;
		}
		myfile.close();
		
		sleep(3); // sleep 3s
	} 
    
    return 0;
}
