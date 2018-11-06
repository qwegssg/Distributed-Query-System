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

// argc is at least 1, because the first string in argv is the command to invoke the program.
int main(int argc, char* argv[]) {
	// cout<<argc<<", "<<argv[1]<<", "<<argv[2]<<", "<<argv[3]<<endl;
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






	return 0;
}