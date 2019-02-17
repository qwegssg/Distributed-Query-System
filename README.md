# Distributed-Query-System
Implemented a model computational offloading where a single client issued three parameters (link ID, file/packet size, and signal power) to the AWS server and expected the reply for the end-to-end delay of the designated link.  
<div align=center><img src="https://github.com/qwegssg/Distributed-Query-System/blob/master/network_illustration.png" alt="Network Illustration" width="500"/></div>
  
## Detailed functionalities  
<b>client.cpp</b>: get the input from user, send the input to AWS server for further searching and computing, and output the delay to terminal after receiving the result from AWS server. 

<b>monitor.cpp</b>: monitor and output all the I/O activities to terminal by receiving corresponding input and output info from AWS server.

<strong>AWS.cpp</strong>: receive the input from client, send the input to storage server A and server B for searching the detailed info about the input. If "No Match" message is received from storage server, then send the "No Match" message back to client and monitor. Once there is a match, then send the input along with the detailed info to computing server C for calculation. When the calculation is done, send the calculation result back to client and monitor separately.

<b>serverA.cpp</b>: get the input from AWS server and search for the corresponding info from database, if there is a match, send the detailed info to the AWS server, if not, send the "No Match" message to AWS server.

<b>serverB.cpp</b>: have exactly the same functionality as serverA.cpp, except for that the search is implemented on a different database.

<b>serverC.cpp</b>: receive the input and detailed info from AWS server, compute the transmission delay, propagation delay and end-to-end delay, send the result back to AWS server.  
  
## highlights  
<b>recvfrom()</b>: the recvfrom() takes a "struct sockaddr*, from" that will indicate where the data came from, and will fill in fromlen with the size of struct sockaddr. (fromlen also must be initialized to be the size of from or struct sockaddr.)
That is, the parameter is just a placeholder and will be filled with the sender's address upon receiving the message. 

<b>htons()</b>: Since different computers use different byte orderings internally for their multibyte integers, (i.e. any integer that's larger than a char.) htons() convert from native byte order to network byte order and back again.  
htons(): host to network short  
ntohs(): network to host short  
E.g.:
some_short == ntohs(htons(some_short)); // this expression is true  
  
<strong>Blocking</strong>: By default, accept(), recv() and recvfrom() functions block. Whenever these functions are called, if there is no data, they block until some data arrives. (However, blocking is relatively a bad idea, since it is such waste of resources that the whole program needs to be waiting for the data.)

Since AWS server is able to handle one request/response from server A and server B at a time, other requests/responses need to be queued at the server site and wait for the server to be freed. Plus, since UDP is <strong>connection-less</strong>, the sequence of receiving data from server A and server B is random, so it is supposed to check the received message's address to determine where it comes from.
  
## Reference  
Beej's Guide to Network Programming Using Internet Sockets  
