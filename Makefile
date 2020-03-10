OBJS = main.cpp Packet.cpp Protocol.cpp Server.cpp Client.cpp Logger.cpp
FLAGS = -g -Wall -pedantic -pthread
EXEC = node
#LIBS = 
#INCLUDE = 

all:
	g++ $(OBJS) -o $(EXEC) $(FLAGS) 

clear:
	rm logs/*
