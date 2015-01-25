#include <boost/unordered_map.hpp>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <ctime>


using namespace std;

template <class T, class U>
typename T::mapped_type* find_ptr(T& c, U v)
{
	typename T::iterator i = c.find(v);
	return i == c.end() ? NULL : &i->second;
}

template <class T, class U>
const typename T::mapped_type* find_ptr(const T& c, U v)
{
	typename T::const_iterator i = c.find(v);
	return i == c.end() ? NULL : &i->second;
}
typedef boost::unordered_map<std::string, std::string> very_long_type_name_t;

very_long_type_name_t CreateMap(){
	very_long_type_name_t very_long_name;
	
	for(int g = 0; g < 1000000; g++){
		std::string he = "hello";
		char concat[100];
		std::string strval = boost::lexical_cast<std::string>(g);

		strcpy(concat, he.c_str());

		strcat(concat, strval.c_str() );
	//	std::cout <<  "concat: " << concat << std::endl;
		very_long_name[concat] = "";
	}
	return very_long_name;
}

int main()
{
	time_t start, end;
//	start = clock();
	very_long_type_name_t very_long_name = CreateMap();
//	end = clock();
//	std::cout << "the time taken to fill the database is: " << end - start << std::endl;
start = time(NULL);
very_long_type_name_t::iterator i;
	for(int c = 0; c < 10000000; c++)
		i = very_long_name.find("hello1");
end = time(NULL);
	float ti = ( (float)end - (float)start );
	std::cout << "Look up time: " << ti << std::endl;
/*  if (i != very_long_name.end())
    cout << i->second << "\n";

  if (very_long_type_name_t::mapped_type* i = find_ptr(very_long_name, 2))
    cout << *i << "\n";
*/
  /*
  very_long_type_name_t::iterator i = very_long_name.find(1); if (i != very_long_name.end()) cout << i->second << "\n";
  if (very_long_type_name_t::mapped_type* i = find_ptr(very_long_name, 1)) cout << *i << "\n";

  auto i = very_long_name.find(1); if (i != very_long_name.end()) cout << i->second << "\n";
  if (auto i = find_ptr(very_long_name, 1)) cout << *i << "\n";
  */
}

