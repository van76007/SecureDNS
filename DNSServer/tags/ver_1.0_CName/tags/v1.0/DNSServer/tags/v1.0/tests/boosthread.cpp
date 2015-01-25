#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#define TRIM_PREDICATE  ((! boost::algorithm::is_print()) || boost::algorithm::is_space())

boost::thread* th1[2];
boost::mutex result_mutex;

boost::mutex mut;
int stack = 0;

void test2();
void test1( std::string  str, std::string a1, std::string a2, std::string a3, std::string a4, std::string a5, std::string a6, std::string a7, std::string a8, std::string f);

void test1( std::string  str, std::string a1, std::string a2, std::string a3, std::string a4, std::string a5, std::string a6, std::string a7, std::string a8, std::string f){
//boost::mutex::scoped_lock lock(result_mutex);
/*	for (int ch = 0; str.c_str()[ch] != '\0'; ++ch) { //keep going until we hit the '\0'
		printf("%c", str.c_str()[ch]); //print a single character and jump to the next line}
	}
*/
	for(int x = 0; x < 10; x++){
		std::cout << "I: " << str << std::endl;
		sleep(1);
	}

}

void test2(){
	//boost::mutex::scoped_lock lock(result_mutex);
	for(int y = 0; y < 10; y++){
		std::cout << "I am in test2" << std::endl;
	//	sleep(1);
	}
	stack = 2;
}

void functiontest(int x, int v){
	std::cout << "x is: " << x << std::endl;
	std::cout << "v: " << v << std::endl;
	if(0 == x){
		for (int i = 0; i<2;i++){
		//	usleep(100);        
			std::cout << "Functiontest:0" << std::endl;
		}
	}
	if(1 == x){
		for (int i = 0; i<2;i++){
		//	usleep(200);        
			std::cout << "Functiontest:1" << std::endl;
		}
	}
	if(2 == x){
		for (int i = 0; i<2;i++){
		//	usleep(300);        
			std::cout << "Functiontest:2" << std::endl;
		}
	}
	
	//sleep(1);	
	//*Blocked = 2;	
	//vec->push_back(1);
	/*for(int x = 0; x < 10;x++){	
		std::cout << "multipletest: " << *Blocked << std::endl;
	//	sleep(1);
	}*/
	//for(int i = 0; i < vec->size(); i++)
	//	std::cout << "MultipleTest is: " << vec->at(i) << std::endl;
	//sleep(2);

}

//boost::mutex mutex;
//pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void multipletest(int i){
	boost::thread_group th;
	std::cout << "Before join i is: " << i << std::endl;
	for (int v = 10; v < 13; v++){
			
		th.add_thread(new boost::thread(functiontest, i, v));
	}
	th.join_all();
	std::cout << "After join i is: " << i << std::endl;
	if(i == 0){
		for(int x = 0; x < 2; x++){
	//		sleep(100);		
			std::cout << "Zero " << std::endl;
		}
	}
	if(i == 1){
		for(int x = 0; x < 2; x++){
	//		sleep(100);		
			std::cout << " One" << std::endl;
		}
	}
	if(i == 2){
		for(int x = 0; x < 2; x++){
	//		sleep(100);		
			std::cout << " Two" << std::endl;
		}
	}
	sleep(1);
	std::cout << "After sleep" << std::endl;
	
}


void anothertest(std::vector<int> *vec){
	std::cout << "Within Another" << std::endl;
	std::cout << "Another vec[i]: " << vec->at(0) << std::endl;
	while(1){
		for(int i = 0; i < vec->size(); i++){
			std::cout << "Another Test is: " << vec->at(i) << std::endl;
		}	
	}
}


//split and Build URL
std::vector<std::string> SplitBuildURL(std::string domain, int BL){

 // std::cout << "domain passed : " << domain << std::endl;
  std::vector<int> Delindex;	
  std::vector<std::string> URLlist;	

  std::string::size_type pos = domain.find_first_of(".");
  Delindex.push_back(pos+1);
  std::string Backdomain = domain;
  while(pos != std::string::npos){
    pos = Backdomain.find_first_of(".", pos + 1);
    Delindex.push_back(pos+1);
    std::cout << "Postion is: " << pos << std::endl;
  }
  URLlist.push_back(domain);
  for(int i = 0; i < (int)Delindex.size() - 2; i++){
    URLlist.push_back(domain.substr( Delindex[i], strlen(domain.c_str())));
    std::cout << "URLlist is: " << URLlist[i] << std::endl;
    if(0 == BL) break;
  }
  std::string h(domain);
  return URLlist;
}

void t1(){

  std::string completeDomain = "www.com";
  std::vector<std::string> NewURLs = SplitBuildURL(completeDomain, 1);
  std::string completeDomain1 = "wsdfkjdshfkjdsf.sdfsdf.sdfsdfsdf.blockercvbcvbcvbxbcv.cfdsgdfgdfgdgf.ru";
  std::vector<std::string> WWWremoved = SplitBuildURL(completeDomain1, 0); 


}

int main(void){
boost::thread th1(t1);
th1.detach();
sleep(5);
return 1;
/*char RBLDomain[100];
std::string domain = "..";
std::string a1 = "1";
std::string a2 = "2";
std::string a3 = "3";
std::string a4 = "4";
std::string a5 = "5";
std::string a6 = "6";
std::string a7 = "7";
std::string a8 = "8";
std::string a9 = "9";
boost::thread_group thgp;

	for(int i = 0; i< 4; i++){
		std::cout <<  i << std::endl;
		thgp.add_thread(new boost::thread(multipletest, i));	
		if(i == 3) sleep(10);
	}
	boost::thread th1(&test1, "am within something!");
	boost::thread th2(&test1, "hello there");
	th1.join();
	th2.join();
*/}//end main
