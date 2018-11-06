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

#define PORT_SERVER_A_UDP 21105

int main() {
	/*
		create UDP socket for server A:
	*/
	// PF_INET or AF_INET?
	int serverSocketA = socket(AF_INET, SOCK_DGRAM, 0);		//referred from Beej's
	if (serverSocketA < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(serverSocketA);
		return 0;
	}
	cout<<"socket result is "<<serverSocketA<<endl;
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in server_A_addr;
    server_A_addr.sin_family = AF_INET;
    server_A_addr.sin_port = htons(PORT_SERVER_A_UDP);
    server_A_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResult = bind(serverSocketA, (struct sockaddr *)&server_A_addr, sizeof server_A_addr);
    if (bindResult < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(serverSocketA);
        return 0;
    }
    // cout<<"bind result is "<<bindResult<<endl;
    cout<<"The Server A is up and running using UDP on port "<<PORT_SERVER_A_UDP<<"."<<endl;

    return 0;



}
