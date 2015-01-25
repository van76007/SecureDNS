/*
Author: dvv@avauntguard.com
Idea: Testbed for boost snippets
*/
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include <vector>
#include <ctime>
#include <string.h>

using namespace std;

void test_pass_vector(std::vector<std::string>& unified_resp, std::string& name) 
{
    unified_resp.push_back("One");
    unified_resp.push_back("Two");
    name = "Van";
}

void test_vector(std::vector<std::string>& uni_resp)
{
    std::string given_name("Dinh");
    test_pass_vector(uni_resp, given_name);
}

void test_pass_boolean(bool* p_bool, unsigned char* buf, size_t buf_len)
{
    *p_bool = true;

    for (int i=0; i<buf_len; i++)
            printf("Get %02x_", buf[i]);
    /*
    if (*p_bool)
        return;
    */

    for (int i=0; i<buf_len; i++)
        buf[i] = 0xAA;
}

int main(int argc, char* argv[]){

        std::cout << "Number of args is: ..." << argc << "\n";
        std::cout << "Begin the test ..." << "\n";
        
        std::string given_name("Dinh");
        std::vector<std::string> unified_resp;
        test_pass_vector(unified_resp, given_name);
      
        for(std::vector<std::string>::iterator j=unified_resp.begin();j!=unified_resp.end();++j)
            std::cout << "Element: " << *j << std::endl;

        std::cout << "Given name is " << given_name << "\n";
	std::cout << "Pass the test test_pass_vector" << "\n";


        std::cout << "Begin the test ..." << "\n";
        
        std::vector<std::string> uni_resp;
        test_vector(uni_resp);
      
        for(std::vector<std::string>::iterator j=uni_resp.begin();j!=uni_resp.end();++j)
            std::cout << "Element: " << *j << std::endl;

	std::cout << "Pass the test test_vector" << "\n"; 

        int label = 1;
        switch(label) {
           case 1:
               int val;
               val = 10;
               if (val == 0) {
                   break;
               }
               else {
                   std::cout << "Do smt else" << "\n"; 
               }
               
               break;
           case 2:
               std::cout << "Case 2" << "\n";
               break;
           default:
               std::cout << "Default case" << "\n";
               break; 
        }

        std::cout << "Pass switch case test" << "\n";
        
        std::cout << "Test passing bool" << "\n";
        bool bool_val = false;
        unsigned char buf[256];
        size_t buf_len = 100;

        for (int i=0; i<buf_len; i++)
            buf[i] = 0x12;

        test_pass_boolean(&bool_val, &buf[0], buf_len);
        if (bool_val)
            std::cout << "bool_val becomes true\n";
        else
            std::cout << "bool_val still false\n";

        for (int i=0; i<buf_len; i++)
            printf("%02x_", buf[i]);
        std::cout << "Pass test passing bool\n"; 

        std::cout << "Begin test print local time\n";

        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        //Format "YYYY-MM-dd hh:mm :ss"
        strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
        
        //puts (buffer);
        std::string time_stamp(buffer, strlen(buffer));
        std::cout << "Time stamp is_" << time_stamp << "_" << std::endl;


        std::cout << "End test print local time\n";      
}
