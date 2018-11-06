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

const char* localHostAddress = "127.0.0.1";s

#define PORT_SERVER_B_UDP 22105

int main() {
	/*
		create UDP socket for server B:
	*/
	int serverSocketB = socket(AF_INET, SOCK_DGRAM, 0);		//referred from Beej's
	if (serverSocketB < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(serverSocketB);
		return 0;
	}
	cout<<"socket result is "<<serverSocketB<<endl;
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in server_B_addr;
    server_B_addr.sin_family = AF_INET;
    server_B_addr.sin_port = htons(PORT_SERVER_B_UDP);
    server_B_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResult = bind(serverSocketB, (struct sockaddr *)&server_B_addr, sizeof server_B_addr);
    if (bindResult < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(serverSocketB);
        return 0;
    }
    // cout<<"bind result is "<<bindResult<<endl;
    cout<<"The Server B is up and running using UDP on port "<<PORT_SERVER_B_UDP<<"."<<endl;

    return 0;



}
