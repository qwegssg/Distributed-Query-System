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

using namespace std;

const char* localHostAddress = "127.0.0.1";

#define PORT_SERVER_B_UDP 22105
#define MAX_DATA_SIZE 100

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
    cout<<"The Server B is up and running using UDP on port <"<<PORT_SERVER_B_UDP<<">."<<endl;
    while (true) {
    	/*
			receive link info from AWS
    	*/
    	struct sockaddr_in storage_addr;            
    	socklen_t fromlen = sizeof storage_addr; 
    	char* bufferID = new char[MAX_DATA_SIZE]; 
    	int recvResult = recvfrom(serverSocketB, bufferID, MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, &fromlen);
    	if (recvResult < 0) {
    		cout<<"Error occurred when receiving link info from AWS!"<<endl;
    		close(serverSocketB);
    		return 0;
    	}
    	cout<<"The Server B received input <"<<bufferID<<">"<<endl;
	    /*
			search the database for an exact match
	    */
	    ifstream file("database_b.csv");
	    if (!file) {
	    	cout<<"Error occurred when connecting to the database!"<<endl;
	    	close(serverSocketB);
	    	return 0;
	    }
    	// convert string into float number 
  		double id = stod(bufferID);
  		bool isFound = false;
  		string m; 
	   	// loook up every line from database
	  	for (string line; getline(file, line);) {
	  		stringstream ss(line);
	  		double i;
	  		vector<double> vect;
	  		// read every number of one line
	  		while (ss >> i) {
	  			if (i != id && !isFound) {
	  				break;
	  			} 
				// if there is a match, extract the detailed link infomation
  				isFound = true;
  				vect.push_back(i);
  				if (ss.peek() == ',') {
  					ss.ignore();
  				}
	  		}
	  		if (isFound) {
	  			m = "1";
	  			cout<<"The server B has found <1> match"<<endl;	  			
	  			sendto(serverSocketB, m.c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, fromlen);
	  			for (int i = 0; i < vect.size(); i++) {
  					sendto(serverSocketB, to_string(vect.at(i)).c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, fromlen);
	  			}
	  			break;
	  		}
	  	}
	  	if (!isFound) {
	  		m = "0";
	  		cout<<"The server B has found <0> match"<<endl;	  		
	  		sendto(serverSocketB, m.c_str(), MAX_DATA_SIZE, 0, (struct sockaddr *)&storage_addr, fromlen);
	  	}
	  	cout<<"The Server B finished sending the output to AWS"<<endl;	
    }

    close(serverSocketB);
    return 0;



}
