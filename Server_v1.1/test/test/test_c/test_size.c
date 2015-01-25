    #include <iostream>
    //
	#include <netinet/in.h>
    #include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <arpa/inet.h>
	#include <string.h>
	#include <string>
	#include <sys/types.h> 
	#include <sys/socket.h>

    using namespace std;
     
    int main(void)
    {
    int a;
    char b;
    unsigned char c;
    signed char d;
    short e;
    unsigned int f;
    long g;
    unsigned long h;
    float i;
    double j;
    long double k;
     
    int *l;
     
    cout << "int - " << sizeof(a) << " bytes" << endl;
    cout << "char - " << sizeof(b) << " bytes" << endl;
    cout << "unsigned char - " << sizeof(c) << " bytes" << endl;
    cout << "signed char - " << sizeof(d) << " bytes" << endl;
    cout << "short - " << sizeof(e) << " bytes" << endl;
    cout << "unsigned int - " << sizeof(f) << " bytes" << endl;
    cout << "long - " << sizeof(g) << " bytes" << endl;
    cout << "unsigned long - " << sizeof(h) << " bytes" << endl;
     
    cout << "float - " << sizeof(i) << " bytes" << endl;
    cout << "double - " << sizeof(j) << " bytes" << endl;
    cout << "long double - " << sizeof(k) << " bytes" << endl;
     
    cout << "type * - " << sizeof(l) << endl;
    }