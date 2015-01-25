/* Creates a datagram server.  The port 
   number is passed as an argument.  This
   server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>



void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void forwardServer(unsigned char * buf){

//manage the forwarding
	unsigned int len; 
	int n;
	struct sockaddr_in forwardingsocket;
	struct sockaddr_in fromNameserver;
	//len = sizeof(forwardingsocket);
	len = sizeof(struct sockaddr_in);
	bzero(&forwardingsocket,len);
	sockaddr_in ForwardAddr;
	sockaddr &serverAddrCast = (sockaddr &)ForwardAddr;
	int sockfdesc = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&ForwardAddr, sizeof(ForwardAddr));
	ForwardAddr.sin_family = AF_INET;
	ForwardAddr.sin_port = htons(53);
	ForwardAddr.sin_addr.s_addr = inet_addr("192.168.230.14");
	unsigned char buffer[1024];
	//std::cout <<"buf: "  << sizeof(buf) << std::endl;
	
	//send DNS query to nameserver
	n=sendto(sockfdesc, buf, 34,0,(const struct sockaddr *)&ForwardAddr, len);
	std::cout << "Packets are being sent" << std::endl;
	
	//receive the DNS response from the nameserver
	recvfrom(sockfdesc, buffer,1024,0,(struct sockaddr *)&fromNameserver, &len);
	std::cout << "After received" << std::endl;
	int y = 0;	
	while(y < 134 ){	
	//	printf("Received is: %02x, %d\n", buffer[y], y);
	//	std::cout << "Buffer: " << strlen(buf) << std::endl;
		//printf("Decimal: %d\n", buf[x]);
		y++;
	}
}

int func(std::string& h){
	h = "I am here";
}

int f(std::string h){
	std::cout << "h is: " << h << std::endl;

}

int main(int argc, char *argv[])
{
	std::string hello;
	func(hello);
	f(hello);
	std::cout << "The string is: " << hello << std::endl;
}

