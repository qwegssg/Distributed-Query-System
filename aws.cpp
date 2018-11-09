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
#include <vector>

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
		create AWS client TCP socket for AWS to communicate with client
	*/
	int awsSocket_client = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (awsSocket_client < 0) {
		cout<<"Error detected when creating the socket for client communication!"<<endl;
		close(awsSocket_client);
		return 0;
	}
	/*
		bind the AWS client socket with a port, referred from Beej's
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
		create AWS monitor TCP socket for AWS to communicate with monitor
	*/
	int awsSocket_monitor = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (awsSocket_monitor < 0) {
		cout<<"Error detected when creating the socket for monitor communication!"<<endl;
		close(awsSocket_monitor);
		return 0;
	}
	/*
		bind the AWS monitor socket with a port, referred from Beej's
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

    // keep listening the upcoming connection from client
    while(true) {


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
        /*
			receive messages from the client
        */
        char* bufferID = new char[MAX_DATA_SIZE];
        char* bufferSize = new char[MAX_DATA_SIZE];
        char* bufferPower = new char[MAX_DATA_SIZE];
        recv(childSocket_client, bufferID,sizeof bufferID, 0);
        recv(childSocket_client, bufferSize, sizeof bufferSize, 0);
        recv(childSocket_client, bufferPower, sizeof bufferPower, 0);
        cout<<"The AWS received link ID=<"<<bufferID<<">, size=<"<<bufferSize<<">, and power=<"<<bufferPower<<"> from the client using TCP over port <"<<PORT_CLIENT_TCP<<">"<<endl;
        /*
            send input messages to monitor over TCP
        */
        int sendMonitorID = send(childSocket_monitor, bufferID, sizeof bufferID, 0);
        int sendMonitorSize = send(childSocket_monitor, bufferSize, sizeof bufferSize, 0);
        int sendMonitorPower = send(childSocket_monitor, bufferPower, sizeof bufferPower, 0);
        if (sendMonitorID < 0 || sendMonitorSize < 0 || sendMonitorPower < 0) {
            cout<<"Error occurred when sending input messages to monitor!"<<endl;
            return 0;
        }
        cout<<"The AWS sent link ID=<"<<bufferID<<">, size=<"<<bufferSize<<">, and power=<"<<bufferPower<<"> to the monitor using TCP over port <"<<PORT_MONITOR_TCP<<">"<<endl;
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
            receive message from storage server A
        */
        char* buffer_A_m = new char[MAX_DATA_SIZE];
        int recvResultA = recvfrom(awsSocket_server, buffer_A_m, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
        if (recvResultA < 0) {
            cout<<"Error occurred when receiving message from server A!"<<endl;
            return 0;
        }

        cout<<buffer_A_m<<", 1111111111"<<endl;



        vector<string> vect_A_data;

        // convert the value to integer
        int m_A = stoi(buffer_A_m);
        if (m_A == 1) {
            // char* buffer_A_ID = new char[MAX_DATA_SIZE];
            // char* buffer_A_bandwidth = new char[MAX_DATA_SIZE];
            // char* buffer_A_length = new char[MAX_DATA_SIZE];
            // char* buffer_A_velocity = new char[MAX_DATA_SIZE];
            // char* buffer_A_power = new char[MAX_DATA_SIZE];
            // recvfrom(awsSocket_server, buffer_A_ID, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            // recvfrom(awsSocket_server, buffer_A_bandwidth, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            // recvfrom(awsSocket_server, buffer_A_length, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            // recvfrom(awsSocket_server, buffer_A_velocity, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            // recvfrom(awsSocket_server, buffer_A_power, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
            
            // receive the link ID, bandwidth, length, velocity and power one by one
            for (int i = 0; i < 5; i++) {
                char* buffer_A = new char[MAX_DATA_SIZE];
                recvfrom(awsSocket_server, buffer_A, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, &tolen_A);
                vect_A_data.push_back(buffer_A);
                free(buffer_A);
                buffer_A = NULL;
            }
            cout<<vect_A_data[0]<<"w4terhbergeryt4hrtg"<<endl;
            cout<<vect_A_data[1]<<"w4terhbergeryt4hrtg"<<endl;
            cout<<"The AWS received <1> matches from Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        } else {
            cout<<"The AWS received <0> matches from Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        }
        /*
            receive message from storage server B
        */
        char* buffer_B_m = new char[MAX_DATA_SIZE];
        int recvResultB = recvfrom(awsSocket_server, buffer_B_m, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_B_addr, &tolen_B);
        if (recvResultB < 0) {
            cout<<"Error occurred when receiving message from server B!"<<endl;
            return 0;
        }

        cout<<buffer_B_m<<", 22222222222222222222"<<endl;


        // convert the value to integer
        int m_B = stoi(buffer_B_m);
        if (m_B == 1) {
            char* buffer_B_ID = new char[MAX_DATA_SIZE];
            char* buffer_B_bandwidth = new char[MAX_DATA_SIZE];
            char* buffer_B_length = new char[MAX_DATA_SIZE];
            char* buffer_B_velocity = new char[MAX_DATA_SIZE];
            char* buffer_B_power = new char[MAX_DATA_SIZE];
            recvfrom(awsSocket_server, buffer_B_ID, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_B_addr, &tolen_B);
            recvfrom(awsSocket_server, buffer_B_bandwidth, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_B_addr, &tolen_B);
            recvfrom(awsSocket_server, buffer_B_length, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_B_addr, &tolen_B);
            recvfrom(awsSocket_server, buffer_B_velocity, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_B_addr, &tolen_B);
            recvfrom(awsSocket_server, buffer_B_power, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_B_addr, &tolen_B);
            cout<<"The AWS received <1> matches from Backend-Server <B> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        } else {
            cout<<"The AWS received <0> matches from Backend-Server <B> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        }



        string outputResult;
        /*
            if data is not found in both databases, aws send "no match found" info to client and monitor
        */
        if (m_A == 0 && m_B == 0) {
            outputResult = "0";
            int sendNoClient = send(childSocket_client, outputResult.c_str(), sizeof outputResult, 0);
            int sendNoMonitor = send(childSocket_monitor, outputResult.c_str(), sizeof outputResult, 0);
            if (sendNoClient < 0 || sendNoMonitor < 0) {
                cout<<"Error occurred when sending 'No Match' info to client and monitor!"<<endl;
                return 0;
            }
            cout<<"The AWS sent No Match to the monitor and the client using TCP over ports <"<<PORT_MONITOR_TCP<<"> and <"<<PORT_CLIENT_TCP<<">, respectively"<<endl;
        }
        else {
        /*
            if there is a match, aws sends input and link info to computing server C over UDP
        */
        struct sockaddr_in server_C_addr;
        server_C_addr.sin_family = AF_INET;
        server_C_addr.sin_port = htons(PORT_SERVER_C_UDP);
        server_C_addr.sin_addr.s_addr = inet_addr(localHostAddress);
        socklen_t tolen_C = sizeof server_C_addr;
        sendto(awsSocket_server, bufferID, sizeof bufferID, 0, (struct sockaddr *)&server_C_addr, tolen_C);
        sendto(awsSocket_server, bufferSize, sizeof bufferSize, 0, (struct sockaddr *)&server_C_addr, tolen_C);
        sendto(awsSocket_server, bufferPower, sizeof bufferPower, 0, (struct sockaddr *)&server_C_addr, tolen_C);


        if (sendResultA < 0) {
            cout<<"Error occurred when sending link info to storage back-server A."<<endl;
            close(awsSocket_server);
            return 0;
        }
        cout<<"The AWS sent link ID=<"<<bufferID<<"> to Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;





            outputResult = "1";
            int sendNoClient = send(childSocket_client, outputResult.c_str(), sizeof outputResult, 0);
            int sendNoMonitor = send(childSocket_monitor, outputResult.c_str(), sizeof outputResult, 0);
            if (sendNoClient < 0 || sendNoMonitor < 0) {
                cout<<"Error occurred when sending 'No Match' info to client and monitor!"<<endl;
                return 0;
            }
            cout<<"There is a match!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
        }

    }


	return 0;

}