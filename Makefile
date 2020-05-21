OBJS = main.cpp Listener.cpp Requester.cpp ./FAT/Blockchain/blockchain.cpp ./FAT/Blockchain/blocks.cpp ./FAT/Blockchain/sha256.cpp
FLAGS = -g -Wall -pedantic -pthread
EXEC = node
#LIBS = 
#INCLUDE = 

all:
	g++ $(OBJS) -o $(EXEC) $(FLAGS) 


