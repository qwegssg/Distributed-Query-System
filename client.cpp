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

// attention: embed the port number for aws server into the client side!
#define PORT_CLIENT_TCP 25105

#define MAX_DATA_SIZE 100

// argc is at least 1, because the first string in argv is the command to invoke the program.
int main(int argc, char* argv[]) {
	// cout<<argc<<", "<<argv[1]<<", "<<argv[2]<<", "<<argv[3]<<endl;
	if (argc < 4) {
		cout<<"Error! please enter the input with correct format!"<<endl;
		return 0;
	}
	/*
		create TCP socket for client:
	*/
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);		//referred from Beej's
	if (clientSocket < 0) {
		cout<<"Error detected when creating the socket!"<<endl;
		close(clientSocket);
		return 0;
	}
	/*
		connect to AWS server, referred from Beej's
	*/
	struct sockaddr_in aws_addr;
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = inet_addr(localHostAddress);
    aws_addr.sin_port = htons(PORT_CLIENT_TCP);
    int connectResult = connect(clientSocket, (struct sockaddr *)&aws_addr, sizeof aws_addr);
	if (connectResult < 0) {
   		cout<<"Error detected when binding the port!"<<endl;
        close(clientSocket);
        return 0;
    }
    cout<<"The client is up and running."<<endl;
    /*
		client send messages to AWS server over TCP
    */
    for (int i = 0; i < 3; i++) {
    	send(clientSocket, argv[i + 1], sizeof argv[i + 1], 0);
    }
	cout<<"The client sent ID=<"<<argv[1]<<">, size=<"<<argv[2]<<">, and power=<"<<argv[3]<<"> to AWS"<<endl;
    /*
		receive results from aws over TCP
    */
    char* bufferResult = new char[MAX_DATA_SIZE];
    recv(clientSocket, bufferResult, sizeof bufferResult, 0);
    int result = stoi(bufferResult);
    if (result == 0) {
    	cout<<"Found no matches for link <"<<argv[1]<<">"<<endl;
    }





	return 0;
}