#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>

using namespace std;

const char* localHostAddress = "127.0.0.1";

#define PORT_SERVER_C_UDP 23105

int main() {
	/*
		create UDP socket for server B:
	*/
	int serverSocketC = socket(AF_INET, SOCK_DGRAM, 0);		//referred from Beej's
	if (serverSocketC < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(serverSocketC);
		return 0;
	}
	cout<<"socket result is "<<serverSocketC<<endl;
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in server_C_addr;
    server_C_addr.sin_family = AF_INET;
    server_C_addr.sin_port = htons(PORT_SERVER_C_UDP);
    server_C_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResult = bind(serverSocketC, (struct sockaddr *)&server_C_addr, sizeof server_C_addr);
    if (bindResult < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(serverSocketC);
        return 0;
    }
    // cout<<"bind result is "<<bindResult<<endl;
    cout<<"The Server C is up and running using UDP on port <"<<PORT_SERVER_C_UDP<<">."<<endl;

    return 0;



}
