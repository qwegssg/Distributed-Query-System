all:
	g++ -std=c++11 -o serverA serverA.cpp
	g++ -std=c++11 -o serverB serverB.cpp
	g++ -std=c++11 -o serverC serverC.cpp
	g++ -std=c++11 -o aws aws.cpp
	g++ -std=c++11 -o monitor monitor.cpp
	g++ -std=c++11 -o client client.cpp

.PHONY: serverA
serverA:
	./serverA

.PHONY: serverB
serverB:
	./serverB

.PHONY: serverC
serverC:
	./serverC

.PHONY: aws
aws:
	./aws

.PHONY: monitor
monitor:
	./monitor
