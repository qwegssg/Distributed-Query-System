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

#define PORT_SERVER_A_UDP 21105
#define MAX_DATA_SIZE 100

int main() {
	/*
		create UDP socket for server A:
	*/
	// PF_INET or AF_INET?
	int serverSocketA = socket(AF_INET, SOCK_DGRAM, 0);		//referred from Beej's
	if (serverSocketA < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(serverSocketA);
		return 0;
	}
	/*
		bind the socket with a port, referred from Beej's
	*/
	struct sockaddr_in server_A_addr;
    server_A_addr.sin_family = AF_INET;
    server_A_addr.sin_port = htons(PORT_SERVER_A_UDP);
    server_A_addr.sin_addr.s_addr = inet_addr(localHostAddress);
	int bindResult = bind(serverSocketA, (struct sockaddr *)&server_A_addr, sizeof server_A_addr);
    if (bindResult < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(serverSocketA);
        return 0;
    }
    // cout<<"bind result is "<<bindResult<<endl;
    cout<<"The Server A is up and running using UDP on port <"<<PORT_SERVER_A_UDP<<">."<<endl;
    while (true) {
    	/*
			receive link info from AWS
    	*/
    	struct sockaddr_in storage_addr;            
    	socklen_t fromlen = sizeof storage_addr; 
    	char* bufferID = new char[MAX_DATA_SIZE]; 
    	int recvResult = recvfrom(serverSocketA, bufferID, sizeof bufferID, 0, (struct sockaddr *)&storage_addr, &fromlen);
    	if (recvResult < 0) {
    		cout<<"Error occurred when receiving link info from AWS!"<<endl;
    		close(serverSocketA);
    		return 0;
    	}
    	cout<<"The Server A received input <"<<bufferID<<">"<<endl;
	    /*
			search the database for an exact match
	    */
	    ifstream file("database_a.csv");
	    if (!file) {
	    	cout<<"Error occurred when connecting to the database!"<<endl;
	    	close(serverSocketA);
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
	  			sendto(serverSocketA, m.c_str(), sizeof m, 0, (struct sockaddr *)&storage_addr, fromlen);
	  			for (int i = 0; i < vect.size(); i++) {
  					sendto(serverSocketA, to_string(vect[i]).c_str(), sizeof to_string(vect[i]), 0, (struct sockaddr *)&storage_addr, fromlen);
	  			}
	  			cout<<"The server A has found <1> match"<<endl;
	  			break;
	  		}
	  	}
	  	if (!isFound) {
	  		m = "0";
	  		sendto(serverSocketA, m.c_str(), sizeof m, 0, (struct sockaddr *)&storage_addr, fromlen);
	  		cout<<"The server A has found <0> match"<<endl;
	  	}


    }

    return 0;
}
