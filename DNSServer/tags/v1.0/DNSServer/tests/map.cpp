#include <iostream>
#include <string>
#include <map>
#include <vector>


using namespace std;

struct IPDesc{
	std::vector<std::string> IPUni;
	std::string Des;
};

int main(void) {
  

/*  typedef std::map<std::string, std::string> col2to3_t;
  typedef std::map<std::string, col2to3_t> col1to2_t;
  
  col1to2_t yourMap;
  
  yourMap["a"]["1"] = "bad";
  yourMap["a"]["2"] = "oops";
  yourMap["b"]["1"] = "good";

  cout << yourMap["a"]["1"] << endl;
  cout << yourMap["a"]["2"] << endl;
  cout << yourMap["b"]["1"] << endl;

  //Non existing will return empty string:
  cout << "empty: " << yourMap["c"]["1"] << endl;
*//*
typedef std::map<std::string, std::string> col2to3;
typedef std::map<std::string, col2to3> col1to2;

  col1to2 M;

std::string ls[] = {"google.com", "bing.com"};
std::cout << "String: " << ls[0] << std::endl;
map<std::string, col2to3>::iterator it;

for(int i = 0; i < 2; i++){

	it = M.find(ls[i]);
	if(it == M.end()){
		std::cout << "Not found" << std::endl;
		M.insert(make_pair(ls[i], col2to3()));	
	}
	//found the hostname
	M[ls[i]].insert(make_pair("123.34.5.78", "Malware in progress"));
	M[ls[i]].insert(make_pair("123.34.5.79", "Malwarvsde in progress"));

}
*/
/*
//std::cout << "M: " << M.find("google.com")->second << std::endl;

col2to3 column = M["google.com"];
//std::cout << column["123.34.5.79"]  << std::endl;

map<std::string,std::string>::iterator it1;
for( it1 = column.begin(); it1 != column.end();it1++){
	std::cout << (*it1).first << "->" << (*it1).second << std::endl; 
}
//std::cout << "Best: " << M["google.com"]["123.34.5.79"] << std::endl;
*/
  multimap<char,int> mymm;
  multimap<char,int>::iterator it;
  pair<multimap<char,int>::iterator,multimap<char,int>::iterator> ret;

  mymm.insert(pair<char,int>('a',10));
  mymm.insert(pair<char,int>('b',20));
  mymm.insert(pair<char,int>('b',30));
  mymm.insert(pair<char,int>('b',40));
  mymm.insert(pair<char,int>('c',50));
  mymm.insert(pair<char,int>('c',60));
  mymm.insert(pair<char,int>('d',60));

  cout << "mymm contains:\n";
  for (char ch='a'; ch<='d'; ch++)
  {
    cout << ch << " =>";
    ret = mymm.equal_range(ch);
    for (it=ret.first; it!=ret.second; ++it)
      cout << " " << (*it).second;
    cout << endl;
  }
  return 0;

}

