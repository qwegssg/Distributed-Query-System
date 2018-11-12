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
#include <iomanip>
#include <sstream>

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
            accept connection from monitor, then the connection must be always on!
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
			receive messages from the client over TCP
        */
        vector<string> client_data;
        for (int i = 0; i < 3; i++) {
            char* buffer_client = new char[MAX_DATA_SIZE];
            int recvResultClient = recv(childSocket_client, buffer_client, MAX_DATA_SIZE, 0);
            if (recvResultClient < 0) {
                cout<<"Error occurred when receiving message from client!"<<endl;
                close(childSocket_client);
                return 0;
            }
            client_data.push_back(buffer_client);
            free(buffer_client);
            buffer_client = NULL;
        }
        cout<<"The AWS received link ID=<"<<client_data[0]<<">, size=<"<<client_data[1]<<">, and power=<"<<client_data[2]<<"> from the client using TCP over port <"<<PORT_CLIENT_TCP<<">"<<endl;
        /*
            send input messages to monitor over TCP
        */
        for (int i = 0; i < 3; i++) {
            int sendResultMonitor = send(childSocket_monitor, client_data[i].c_str(), MAX_DATA_SIZE, 0);  
            if (sendResultMonitor < 0) {
                cout<<"Error occurred when sending input messages to monitor!"<<endl;
                close(childSocket_monitor);
                return 0;
            }
        }
        cout<<"The AWS sent link ID=<"<<client_data[0]<<">, size=<"<<client_data[1]<<">, and power=<"<<client_data[2]<<"> to the monitor using TCP over port <"<<PORT_MONITOR_TCP<<">"<<endl;
        /*
            send link info to server A, referred from Beej's
        */
        struct sockaddr_in server_A_addr;
        server_A_addr.sin_family = AF_INET;
        server_A_addr.sin_port = htons(PORT_SERVER_A_UDP);
        server_A_addr.sin_addr.s_addr = inet_addr(localHostAddress);
        socklen_t tolen_A = sizeof server_A_addr;
        int sendResultA = sendto(awsSocket_server, client_data[0].c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&server_A_addr, tolen_A);
        if (sendResultA < 0) {
            cout<<"Error occurred when sending link info to storage back-server A."<<endl;
            close(awsSocket_server);
            return 0;
        }
        cout<<"The AWS sent link ID=<"<<client_data[0]<<"> to Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        /*
            send link info to server B, referred from Beej's
        */
        struct sockaddr_in server_B_addr;
        server_B_addr.sin_family = AF_INET;
        server_B_addr.sin_port = htons(PORT_SERVER_B_UDP);
        server_B_addr.sin_addr.s_addr = inet_addr(localHostAddress);
        socklen_t tolen_B = sizeof server_B_addr;
        int sendResultB = sendto(awsSocket_server, client_data[0].c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&server_B_addr, tolen_B);
        if (sendResultB < 0) {
            cout<<"Error occurred when sending link info to storage back-server B."<<endl;
            close(awsSocket_server);
            return 0;
        }
        cout<<"The AWS sent link ID=<"<<client_data[0]<<"> to Backend-Server <B> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
        /*
            receive messages from storage server A & B
        */
        struct sockaddr_in storage_addr;            
        socklen_t fromlen = sizeof storage_addr; 
        bool isMatch_A = false;
        bool isMatch_B = false;
        vector<double> vect_A_data;
        vector<double> vect_B_data;
        for (int i = 0; i < 2; i++) {
            char* buffer_m = new char[MAX_DATA_SIZE];
            int recvResultM = recvfrom(awsSocket_server, buffer_m, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
            if (recvResultM < 0) {
                cout<<"Error occurred when receiving message from storage server!"<<endl;
                close(awsSocket_server);
                return 0;
            }
            stringstream ss(buffer_m);
            bool isChecked = false;
            double num;
            while (ss >> num) {
                // the server B sends back "No Match" message
                if (!isChecked && num == 20) {
                    isChecked = true;
                    cout<<"The AWS received <0> matches from Backend-Server <B> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
                    break;
                }
                // the server B finds a match and sends back the detailed info
                if (!isChecked && num == 21) {
                    isChecked = true;
                    isMatch_B = true;
                    cout<<"The AWS received <1> matches from Backend-Server <B> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
                    if (ss.peek() == ',') {
                        ss.ignore();
                    }
                    continue;
                }
                // the server A sends back "No Match" message
                if (!isChecked && num == 10) {
                    isChecked = true;
                    cout<<"The AWS received <0> matches from Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
                    break;
                }
                // the server A finds a match and sends back the detailed info
                if (!isChecked && num == 11) {
                    isChecked = true;
                    isMatch_A = true;
                    cout<<"The AWS received <1> matches from Backend-Server <A> using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
                    if (ss.peek() == ',') {
                        ss.ignore();
                    }
                    continue;
                }
                // store the link ID, bandwidth, length, velocity and power one by one
                if (isMatch_A) {
                    vect_A_data.push_back(num);
                }
                if (isMatch_B) {
                    vect_B_data.push_back(num);
                }
                if (ss.peek() == ',') {
                    ss.ignore();
                }
            }
        }
        /*
            if data is not found in both databases, aws send "no match found" info to client and monitor.
            if there is a match, then communicate computing serverC
        */
        string outputResult;
        if (!isMatch_A && !isMatch_B) {
            outputResult = "0";
            int sendNoClient = send(childSocket_client, outputResult.c_str(), MAX_DATA_SIZE, 0);
            int sendNoMonitor = send(childSocket_monitor, outputResult.c_str(), MAX_DATA_SIZE, 0);
            if (sendNoClient < 0 || sendNoMonitor < 0) {
                cout<<"Error occurred when sending 'No Match' info to client and monitor!"<<endl;
                return 0;
            }
            cout<<"The AWS sent No Match to the monitor and the client using TCP over ports <"<<PORT_MONITOR_TCP<<"> and <"<<PORT_CLIENT_TCP<<">, respectively"<<endl;
        }
        /*
            if there is a match, AWS sends input and link info to computing server C over UDP
        */
        else {
            struct sockaddr_in server_C_addr;
            server_C_addr.sin_family = AF_INET;
            server_C_addr.sin_port = htons(PORT_SERVER_C_UDP);
            server_C_addr.sin_addr.s_addr = inet_addr(localHostAddress);
            socklen_t tolen_C = sizeof server_C_addr;
            for (int i = 0; i < 3; i++) {
                int sendInputC = sendto(awsSocket_server, client_data[i].c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&server_C_addr, tolen_C);
                if (sendInputC < 0) {
                    cout<<"Error occurred when sending data to server C!"<<endl;
                    close(awsSocket_server);
                    return 0;
                }
            }
            // attention: start sending from the second element of vector, leave the link ID since it is duplicated
            if (isMatch_A) {
                for(int i = 0; i < 4; i++) {
                    int sendResultC = sendto(awsSocket_server, to_string(vect_A_data[i + 1]).c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&server_C_addr, tolen_C);
                    if (sendResultC < 0) {
                        cout<<"Error occurred when sending data to server C!"<<endl;
                        close(awsSocket_server);
                        return 0;
                    }
                }
            } 
            else if (isMatch_B) {
                for(int i = 0; i < 4; i++) {
                    int sendResultC = sendto(awsSocket_server, to_string(vect_B_data[i + 1]).c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&server_C_addr, tolen_C);
                    if (sendResultC < 0) {
                        cout<<"Error occurred when sending data to server C!"<<endl;
                        close(awsSocket_server);
                        return 0;
                    }
                }
            }
            cout<<"The AWS sent link ID=<"<<client_data[0]<<">, size=<"<<client_data[1]<<">, power=<"<<client_data[1]<<">, and link information to Backend-Server C using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
            /*
                receive the calculation result from server C over UDP
            */
            vector<string> vect_C_data;
            for (int i = 0; i < 3; i++) {
                char* buffer_C = new char[MAX_DATA_SIZE];
                int recvDataC = recvfrom(awsSocket_server, buffer_C, MAX_DATA_SIZE, 0, (struct sockaddr *)&server_C_addr, &tolen_C);
                if (recvDataC < 0) {
                    cout<<"Error occurred when receiving messages from server C!"<<endl;
                    close(awsSocket_server);
                    return 0;
                }
                vect_C_data.push_back(buffer_C);
                free(buffer_C);
                buffer_C = NULL;
            }
            cout<<"The AWS received outputs from Backend-Server C using UDP over port <"<<PORT_AWS_UDP<<">"<<endl;
            outputResult = "1";
            int sendResultClient = send(childSocket_client, outputResult.c_str(), MAX_DATA_SIZE, 0);
            int sendDelayClient = send(childSocket_client, vect_C_data[2].c_str(), MAX_DATA_SIZE, 0);
            if (sendResultClient < 0 || sendDelayClient < 0) {
                cout<<"Error occurred when sending calculation result to client!"<<endl;
                close(childSocket_client);
                return 0;
            }
            cout<<"The AWS sent delay=<"<<setiosflags(ios::fixed)<<setprecision(2)<<stod(vect_C_data[2])<<">ms to the client using TCP over port <"<<PORT_CLIENT_TCP<<">"<<endl;
            int sendResultMonitor = send(childSocket_monitor, outputResult.c_str(), MAX_DATA_SIZE, 0);
            if (sendResultMonitor < 0) {
                cout<<"Error occurred when sending calculation result to monitor!"<<endl;
                close(sendResultMonitor);
                return 0;
            }
            for (int i = 0; i < 3; i++) {
                int sendDelayMonitor = send(childSocket_monitor, vect_C_data[i].c_str(), MAX_DATA_SIZE, 0);
                if (sendDelayMonitor < 0) {
                    cout<<"Error occurred when sending calculation result to monitor!"<<endl;
                    close(childSocket_monitor);
                    return 0;
                }
            }
            cout<<"The AWS sent detailed results to the monitor using TCP over port <"<<PORT_MONITOR_TCP<<">"<<endl;
        }
    }

    close(awsSocket_server);
    close(awsSocket_client);
    close(awsSocket_monitor);
	return 0;
}