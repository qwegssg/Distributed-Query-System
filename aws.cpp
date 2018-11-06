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

#define PORT_CLIENT_TCP 25105
#define PORT_MONITOR_TCP 26105
#define QUEUE_LIMIT 10

int main() {
	/*
		create TCP socket for AWS to communicate with client
	*/
	int awsSocket_client = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (awsSocket_client < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(awsSocket_client);
		return 0;
	}
	// cout<<awsSocket_client<<endl;
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT_CLIENT_TCP);
    client_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResultClient = bind(awsSocket_client, (struct sockaddr *)&client_addr, sizeof client_addr);
    if (bindResultClient < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(awsSocket_client);
        return 0;
    }
    /*
		create TCP socket for AWS to communicate with monitor
	*/
	int awsSocket_monitor = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (awsSocket_monitor < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(awsSocket_monitor);
		return 0;
	}
	// cout<<awsSocket_monitor<<endl;
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in monitor_addr;
    monitor_addr.sin_family = AF_INET;
    monitor_addr.sin_port = htons(PORT_MONITOR_TCP);
    monitor_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResultMonitor = bind(awsSocket_monitor, (struct sockaddr *)&monitor_addr, sizeof monitor_addr);
    if (bindResultMonitor < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(awsSocket_monitor);
        return 0;
    }
    cout<<"The AWS is up and running."<<endl;
    /*
		listen connections from client and monitor
    */
    int listenClientResult = listen(awsSocket_client, QUEUE_LIMIT);
    int listenMonitorResult = listen(awsSocket_monitor, QUEUE_LIMIT);


    while(true) {

    }


	return 0;

}