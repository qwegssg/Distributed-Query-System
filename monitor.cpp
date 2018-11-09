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

// attention: embed the port number for aws server into the monitor side!
#define PORT_MONITOR_TCP 26105

#define MAX_DATA_SIZE 100

int main() {
	// monitor is always on once started
    while(true) {



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

    	/*
			receive messages from AWS over TCP
    	*/
    	char* bufferID = new char[MAX_DATA_SIZE];
        char* bufferSize = new char[MAX_DATA_SIZE];
        char* bufferPower = new char[MAX_DATA_SIZE];
        recv(monitorSocket, bufferID, sizeof bufferID, 0);
        recv(monitorSocket, bufferSize, sizeof bufferSize, 0);
        recv(monitorSocket, bufferPower, sizeof bufferPower, 0);
        cout<<"The monitor received link ID=<"<<bufferID<<">, size=<"<<bufferSize<<">, and power=<"<<bufferPower<<"> from the AWS"<<endl;
        /*
			receive results from aws over TCP
        */
        char* bufferResult = new char[MAX_DATA_SIZE];
        recv(monitorSocket, bufferResult, sizeof bufferResult, 0);
        int result = stoi(bufferResult);
        if (result == 0) {
        	cout<<"Found no matches for link <"<<bufferID<<">"<<endl;
        }





        memset(bufferID, 0, MAX_DATA_SIZE);
        memset(bufferSize, 0, MAX_DATA_SIZE);
        memset(bufferPower, 0, MAX_DATA_SIZE);
        memset(bufferResult, 0, MAX_DATA_SIZE);
    }

    return 0;
}