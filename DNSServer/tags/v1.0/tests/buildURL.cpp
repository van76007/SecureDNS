#include <vector>
#include <iostream>
#include <string>
#include <string.h>
#include <algorithm>

using namespace std;

inline vector<std::string> split( const std::string& s, const string& f );

inline vector<std::string> split( const std::string& s, const string& f ) {
    vector<string> temp;
    if ( f.empty() ) {
        temp.push_back( s );
        return temp;
    }
    typedef std::string::const_iterator iter;
    const iter::difference_type f_size( distance( f.begin(), f.end() ) );
    iter i( s.begin() );
    for ( iter pos; ( pos = search( i , s.end(), f.begin(), f.end() ) ) != s.end(); ) {
        temp.push_back( string( i, pos ) );
        advance( pos, f_size );
        i = pos;
    }
    temp.push_back( std::string( i, s.end() ) );
    return temp;
}




int main(void){

	vector<int> Delindex;	
	vector<std::string> URLlist;	
URLlist.clear();
URLlist.clear();
URLlist.clear();
//std::cout << "URL: " << URLlist[0] << std::endl;
/*	std::string domain = "www.www.topupdaters.ru";
	string::size_type pos = domain.find_first_of(".");

	Delindex.push_back(pos+1);
	std::string Backdomain = domain;
	while(pos != string::npos){
		pos = Backdomain.find_first_of(".", pos + 1);
		Delindex.push_back(pos+1);
		std::cout << pos << std::endl;
	}
	int h = 0;
	for(int i = 0; i < Delindex.size() - 2; i++){
		URLlist.push_back(domain.substr( Delindex[i], strlen(domain.c_str())));
		std::cout << URLlist[i] << std::endl;
		if(h == 0) break;	
 	}
*/

 //std::vector<int> Delindex;	
 // std::vector<std::string> URLlist;	
  std::string domain = "www.topupdaters.ru";
int BL = 1;
  std::string::size_type pos = domain.find_first_of(".");
  Delindex.push_back(pos+1);
  std::string Backdomain = domain;
  while(pos != std::string::npos){
    pos = Backdomain.find_first_of(".", pos + 1);
    Delindex.push_back(pos+1);
    //std::cout << pos << std::endl;
  }
  URLlist.push_back(domain);
  for(int i = 0; i < (int)Delindex.size() - 2; i++){
    URLlist.push_back(domain.substr( Delindex[i], strlen(domain.c_str())));
    //std::cout << URLlist[i] << std::endl;
    if(0 == BL) break;
  }
//  return URLlist;	
std::cout << URLlist[0] << std::endl;
std::cout << URLlist[1] << std::endl;
	return 1;
}
