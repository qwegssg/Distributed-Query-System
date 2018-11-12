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
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

const char* localHostAddress = "127.0.0.1";

#define PORT_SERVER_C_UDP 23105
#define MAX_DATA_SIZE 100

int main() {
	/*
		create UDP socket for server B:
	*/
	int serverSocketC = socket(AF_INET, SOCK_DGRAM, 0);		//referred from Beej's
	if (serverSocketC < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(serverSocketC);
		return 0;
	}
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in server_C_addr;
    server_C_addr.sin_family = AF_INET;
    server_C_addr.sin_port = htons(PORT_SERVER_C_UDP);
    server_C_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResult = bind(serverSocketC, (struct sockaddr *)&server_C_addr, sizeof server_C_addr);
    if (bindResult < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(serverSocketC);
        return 0;
    }
    // cout<<"bind result is "<<bindResult<<endl;
    cout<<"The Server C is up and running using UDP on port <"<<PORT_SERVER_C_UDP<<">."<<endl;
    while (true) {
    	/*
			receive messages from AWS over UDP
    	*/
    	struct sockaddr_in storage_addr;            
    	socklen_t fromlen = sizeof storage_addr; 
		char* bufferID = new char[MAX_DATA_SIZE]; 
		char* bufferSize = new char[MAX_DATA_SIZE];
		char* bufferPower = new char[MAX_DATA_SIZE];    	
		char* bufferBandwidth = new char[MAX_DATA_SIZE];  
		char* bufferLength = new char[MAX_DATA_SIZE];
		char* bufferVelocity = new char[MAX_DATA_SIZE];
		char* bufferNoisePower = new char[MAX_DATA_SIZE];	 
		recvfrom(serverSocketC, bufferID, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
		recvfrom(serverSocketC, bufferSize, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
		recvfrom(serverSocketC, bufferPower, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
		recvfrom(serverSocketC, bufferBandwidth, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
		recvfrom(serverSocketC, bufferLength, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
		recvfrom(serverSocketC, bufferVelocity, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
		recvfrom(serverSocketC, bufferNoisePower, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
		cout<<"The Server C received link information of link <"<<bufferID<<">, file size <"<<bufferSize<<">, and signal power <"<<bufferPower<<">"<<endl;
		double SNR = pow(10, (stod(bufferPower) - stod(bufferNoisePower)) / 10);
		double capacity = log2(1 + SNR) * (stod(bufferBandwidth) * 1000000);
		double Ttran = (stod(bufferSize) / capacity) * 1000;
		double Tprop = (stod(bufferLength) * 1000 / (stod(bufferVelocity) * 10000000)) * 1000;
		double Txfr = Ttran + Tprop;
		cout<<"The server C finished the calculation for link <"<<bufferID<<">"<<endl;
		/*
			send the calculation result to AWS over UDP
		*/
		sendto(serverSocketC, to_string(Ttran).c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, fromlen);
		sendto(serverSocketC, to_string(Tprop).c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, fromlen);
		sendto(serverSocketC, to_string(Txfr).c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, fromlen);
    	cout<<"The Server C finished sending the output to AWS"<<endl;
    }

    close(serverSocketC);
    return 0;



}
