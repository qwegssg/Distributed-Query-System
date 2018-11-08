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
#include <typeinfo>

using namespace std;

const char* localHostAddress = "127.0.0.1";

// embeded server side port number
#define PORT_SERVER_A_UDP 21105
#define PORT_SERVER_B_UDP 22105
#define PORT_SERVER_C_UDP 23105

#define PORT_AWS_UDP 24105
#define PORT_CLIENT_TCP 25105
#define PORT_MONITOR_TCP 26105
#define QUEUE_LIMIT 10
#define MAX_DATA_SIZE 100

int main() {
	/*
		create TCP socket for AWS to communicate with client
	*/
	int awsSocket_client = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (awsSocket_client < 0) {
		cout<<"Error detected when creating the socket for client communication!"<<endl;
		close(awsSocket_client);
		return 0;
	}
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT_CLIENT_TCP);
    client_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResultClient = bind(awsSocket_client, (struct sockaddr *)&client_addr, sizeof client_addr);
    if (bindResultClient < 0) {
   		cout<<"Error detected when binding the port for client communication!"<<endl;
        close(awsSocket_client);
        return 0;
    }
    /*
		create TCP socket for AWS to communicate with monitor
	*/
	int awsSocket_monitor = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (awsSocket_monitor < 0) {
		cout<<"Error detected when creating the socket for monitor communication!"<<endl;
		close(awsSocket_monitor);
		return 0;
	}
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in monitor_addr;
    monitor_addr.sin_family = AF_INET;
    monitor_addr.sin_port = htons(PORT_MONITOR_TCP);
    monitor_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResultMonitor = bind(awsSocket_monitor, (struct sockaddr *)&monitor_addr, sizeof monitor_addr);
    if (bindResultMonitor < 0) {
   		cout<<"Error detected when binding the port for monitor communication!"<<endl;
        close(awsSocket_monitor);
        return 0;
    }
    /*
        create UDP socket for communication with back-servers
    */
    int awsSocket_server = socket(AF_INET, SOCK_DGRAM, 0);     //referred from Beej's
    if (awsSocket_server < 0) {
        cout<<"Error detected when creating the socket for back-server communication!"<<endl;
        close(awsSocket_server);
        return 0;
    }
    /*
        bind the socket with a port, referred from Beej's
    */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_AWS_UDP);
    server_addr.sin_addr.s_addr = inet_addr(localHostAddress);
    int bindResultServer = bind(awsSocket_server, (struct sockaddr *)&server_addr, sizeof server_addr);
    if (bindResultServer < 0) {
        cout<<"Error detected when binding the port for back-server communication!"<<endl;
        close(awsSocket_server);
        return 0;
    }
    cout<<"The AWS is up and running."<<endl;
    /*
		listen connections from client and monitor
    */
    int listenClientResult = listen(awsSocket_client, QUEUE_LIMIT);
    int listenMonitorResult = listen(awsSocket_monitor, QUEUE_LIMIT);
    /*
		accept connection from monitor, then the connection is always on!
    */
	socklen_t monitor_addr_size = sizeof monitor_addr;
    int childSocket_monitor = accept(awsSocket_monitor, (struct sockaddr *)&monitor_addr, &monitor_addr_size);
    if (childSocket_monitor < 0) {
        cout<<"Error detected when accepting the connection with monitor!"<<endl;
        close(childSocket_monitor);
        return 0;
    }
    // keep listening the upcoming connection from client
    while(true) {
    	/*
			accept for the incoming client(s)!
    	*/
    	socklen_t client_addr_size = sizeof client_addr;
        int childSocket_client = accept(awsSocket_client, (struct sockaddr *)&client_addr, &client_addr_size);
        if (childSocket_client < 0) {
            cout<<"Error detected when accepting the connection with client!"<<endl;
            close(childSocket_client);
            return 0;
        }
        // cout<<childSocket_client<<", "<<childSocket_monitor<<endl;
        /*
			receive messages from the client
        */
        char* bufferID = new char[MAX_DATA_SIZE];
        char* bufferSize = new char[MAX_DATA_SIZE];
        char* bufferPower = new char[MAX_DATA_SIZE];
        recv(childSocket_client, bufferID, sizeof bufferID, 0);
        recv(childSocket_client, bufferSize, sizeof bufferSize, 0);
        recv(childSocket_client, bufferPower, sizeof bufferPower, 0);
        cout<<"The AWS received link ID=<"<<bufferID<<">, size=<"<<bufferSize<<">, and power=<"<<bufferPower<<"> from the client using TCP over port <"<<PORT_CLIENT_TCP<<">"<<endl;
        /*
            send link info to server A, referred from Beej's
        */
        struct sockaddr_in server_A_addr;
        server_A_addr.sin_family = AF_INET;
        server_A_addr.sin_port = htons(PORT_SERVER_A_UDP);
        server_A_addr.sin_addr.s_addr = inet_addr(localHostAddress);
        socklen_t tolen_A = sizeof server_A_addr;
        int sendResultA = sendto(awsSocket_server, bufferID, sizeof bufferID, 0, (struct sockaddr *)&server_A_addr, tolen_A);
        if (sendResultA < 0) {
            cout<<"Error occurred when sending link info to storage back-server A."<<endl;
            close(awsSocket_server);
            return 0;
        }
        cout<<"The AWS sent link ID=<"<<bufferID<<"> to Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
         /*
            send link info to server B, referred from Beej's
        */
        struct sockaddr_in server_B_addr;
        server_B_addr.sin_family = AF_INET;
        server_B_addr.sin_port = htons(PORT_SERVER_B_UDP);
        server_B_addr.sin_addr.s_addr = inet_addr(localHostAddress);
        socklen_t tolen_B = sizeof server_B_addr;
        int sendResultB = sendto(awsSocket_server, bufferID, sizeof bufferID, 0, (struct sockaddr *)&server_B_addr, tolen_B);
        if (sendResultB < 0) {
            cout<<"Error occurred when sending link info to storage back-server B."<<endl;
            close(awsSocket_server);
            return 0;
        }
        cout<<"The AWS sent link ID=<"<<bufferID<<"> to Backend-Server <B> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        /*
            receive message from storage server
        */
        char* buffer_A_m = new char[MAX_DATA_SIZE];
        int recvResultA = recvfrom(awsSocket_server, buffer_A_m, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
        if (recvResultA < 0) {
            cout<<"Error occurred when receiving message from server A!"<<endl;
        }
        // convert the value to integer
        int m = stoi(buffer_A_m);
        if (m == 1) {
            char* buffer_A_ID = new char[MAX_DATA_SIZE];
            char* buffer_A_bandwidth = new char[MAX_DATA_SIZE];
            char* buffer_A_length = new char[MAX_DATA_SIZE];
            char* buffer_A_velocity = new char[MAX_DATA_SIZE];
            char* buffer_A_power = new char[MAX_DATA_SIZE];
            recvfrom(awsSocket_server, buffer_A_ID, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            recvfrom(awsSocket_server, buffer_A_bandwidth, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            recvfrom(awsSocket_server, buffer_A_length, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            recvfrom(awsSocket_server, buffer_A_velocity, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            recvfrom(awsSocket_server, buffer_A_power, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            cout<<"The AWS received <1> matches from Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
            // cout<<buffer_A_ID<<endl;
            // cout<<buffer_A_bandwidth<<endl;
            // cout<<buffer_A_length<<endl;
            // cout<<buffer_A_velocity<<endl;
            // cout<<buffer_A_power<<endl;
        } else {
            cout<<"The AWS received <0> matches from Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        }

    }


	return 0;

}