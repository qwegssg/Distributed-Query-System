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

#define PORT_MONITOR_TCP 26105

int main() {
	/*
		create TCP socket for monitor:
	*/
	int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (monitorSocket < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(monitorSocket);
		return 0;
	}
	/*
		connect to AWS server, referred from Beej's
	*/
	struct sockaddr_in aws_addr;
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = inet_addr(localHostAddress);
    aws_addr.sin_port = htons(PORT_MONITOR_TCP);
    int connectResult = connect(monitorSocket, (struct sockaddr *)&aws_addr, sizeof aws_addr);
	if (connectResult < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(monitorSocket);
        return 0;
    }
    cout<<"The monitor is up and running."<<endl;
    // monitor is always on once started
    while(true) {

    }

    return 0;
}