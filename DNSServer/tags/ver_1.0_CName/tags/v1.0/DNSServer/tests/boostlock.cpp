#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <sys/types.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdio>

boost::mutex m_mutex;
boost::condition_variable m_cond;

boost::system_time timeout = boost::get_system_time() +
                             boost::posix_time::milliseconds(2500);

int testFunc(int i, int *bl)
{
        std::cout << "int i: " << i << std::endl;
	sleep(1);
	boost::unique_lock<boost::mutex> lock(m_mutex);
	
	if(i == 2){	
		*bl = 100;
		m_cond.notify_one();
		return 1;
	}
	else{
		*bl = 2;
		return 0;	
	}
	std::cout << *bl << std::endl;

	return 0;
}


void MonitorThread(int *bl, std::string *BLfin){
/*	boost::unique_lock<boost::mutex> lock(m_mutex);
	while (*bl != 100){
		std::cout << "Before bl is: " << *bl << std::endl; 
	        int x = m_cond.timed_wait(lock, timeout);
		*BLfin = *bl;
		std::cout << "after bl is: " << *bl << std::endl; 
		std::cout << "x is: " << x << std::endl;
		std::cout << "bl????????? is: " << *bl << std::endl;
		break;
	}
*/
*BLfin = "Hello";
}

struct timeval start, end;
long mtime, seconds, useconds;

void func(){
	
}


void f(std::string h){ 
	std::cout << "f is: " << h << std::endl; 

}

int main(void){
 	boost::thread_group th1;
	th1.add_thread(new boost::thread(func));

	int bl = 0;
	std::string BLFin = "";
	boost::thread_group thgp;
/*	gettimeofday(&start, NULL);
	usleep(2000);
*/	thgp.add_thread(new boost::thread(MonitorThread, &bl, &BLFin));
	/*thgp.join_all();
	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	printf("Elaps time: %ld milliseconds\n", mtime);       

	sleep(5);
*/	for(int i = 0; i < 4; i++){
			thgp.add_thread(new boost::thread(testFunc, i, &bl));	
		}   
       
	thgp.join_all();

        f(BLFin);
	std::cout << "bl ???? : " << BLFin << std::endl;
	return 1;
}
