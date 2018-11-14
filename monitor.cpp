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
#include <vector>
#include <iomanip>

using namespace std;

const char* localHostAddress = "127.0.0.1";

// attention: embed the port number for aws server into the monitor side!
#define PORT_MONITOR_TCP 26105

#define MAX_DATA_SIZE 100

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
    	/*
			receive messages from AWS over TCP
    	*/
    	vector<string> monitor_data;
    	for (int i = 0; i < 3; i++) {
	    	char* buffer = new char[MAX_DATA_SIZE];
	        int recvResult = recv(monitorSocket, buffer, MAX_DATA_SIZE, 0);
	        if (recvResult < 0) {
	        	cout<<"Error occurred when receiving data from AWS!"<<endl;
	        }
	        monitor_data.push_back(buffer);
            free(buffer);
            buffer = NULL;
    	}
        cout<<"The monitor received link ID=<"<<monitor_data[0]<<">, size=<"<<monitor_data[1]<<">, and power=<"<<monitor_data[2]<<"> from the AWS"<<endl;
        /*
			receive results from aws over TCP
        */
        char* bufferResult = new char[MAX_DATA_SIZE];
        recv(monitorSocket, bufferResult, MAX_DATA_SIZE, 0);
        int result = stoi(bufferResult);
        if (result == 0) {
        	cout<<"Found no matches for link <"<<monitor_data[0]<<">"<<endl;
        }
        else if (result == 1) {
        	vector<string> monitor_delay;
    		for (int i = 0; i < 3; i++) {
		    	char* bufferDelay = new char[MAX_DATA_SIZE];
		        int recvDelay = recv(monitorSocket, bufferDelay, MAX_DATA_SIZE, 0);
		        if (recvDelay < 0) {
		        	cout<<"Error occurred when receiving calculation result from AWS!"<<endl;
		        }
		        monitor_delay.push_back(bufferDelay);
	            free(bufferDelay);
	            bufferDelay = NULL;
    		}
    		cout<<"The result for link <"<<monitor_data[0]<<">:"<<endl;
			cout<<"Tt = <"<<setiosflags(ios::fixed)<<setprecision(2)<<stod(monitor_delay[0])<<">ms,"<<endl;
			cout<<"Tp = <"<<setiosflags(ios::fixed)<<setprecision(2)<<stod(monitor_delay[1])<<">ms,"<<endl;
			cout<<"Delay = <"<<setiosflags(ios::fixed)<<setprecision(2)<<stod(monitor_delay[2])<<">ms"<<endl;
        }
    }

    return 0;
}
