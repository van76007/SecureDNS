#include <iostream>
#include <string>
#include <set>
#include <sstream>
#include <exception>
#include <fstream>

#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

namespace pod = boost::program_options::detail;

int main() 
{  
    std::ifstream config("config.ini");
    if(!config)
    {
        std::cerr<<"error"<<std::endl;
        return 1;
    }

    //parameters
    std::set<std::string> options;
    std::map<std::string, std::string> parameters;
    options.insert("*");
    
    try
    {      
        for (pod::config_file_iterator i(config, options), e ; i != e; ++i)
        {
            std::cout << i->string_key <<" "<<i->value[0] << std::endl;
            parameters[i->string_key] = i->value[0];
        }
        std::cout << parameters["port_protection_plus_1"] << std::endl;
    }
    catch(std::exception& e)    
    {
        std::cerr<<"Exception: "<<e.what()<<std::endl;
    }
}