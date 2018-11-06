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
#define MAX_BUFFER_SIZE 2048

#define PORT_CLIENT_TCP 25105
#define PORT_MONITOR_TCP 26105

int main() {
	/*
		create TCP socket for AWS to communicate with client
	*/
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (clientSocket < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(clientSocket);
		return 0;
	}
	// cout<<clientSocket<<endl;
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT_CLIENT_TCP);
    client_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResultClient = bind(clientSocket, (struct sockaddr *)&client_addr, sizeof client_addr);
    if (bindResultClient < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(clientSocket);
        return 0;
    }
    /*
		create TCP socket for AWS to communicate with monitor
	*/
	int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (monitorSocket < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(monitorSocket);
		return 0;
	}
	// cout<<monitorSocket<<endl;
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in monitor_addr;
    monitor_addr.sin_family = AF_INET;
    monitor_addr.sin_port = htons(PORT_MONITOR_TCP);
    monitor_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResultMonitor = bind(monitorSocket, (struct sockaddr *)&monitor_addr, sizeof monitor_addr);
    if (bindResultMonitor < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(monitorSocket);
        return 0;
    }

    cout<<"The AWS is up and running."<<endl;
	return 0;

}