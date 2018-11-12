all:
	g++ -std=c++11 -o serverAoutput serverA.cpp
	g++ -std=c++11 -o serverBoutput serverB.cpp
	g++ -std=c++11 -o serverCoutput serverC.cpp
	g++ -std=c++11 -o awsoutput aws.cpp
	g++ -std=c++11 -o monitoroutput monitor.cpp
	g++ -std=c++11 -o client client.cpp

.PHONY: serverA
serverA:
	./serverAoutput

.PHONY: serverB
serverB:
	./serverBoutput

.PHONY: serverC
serverC:
	./serverCoutput

.PHONY: aws
aws:
	./awsoutput

.PHONY: monitor
monitor:
	./monitoroutput
