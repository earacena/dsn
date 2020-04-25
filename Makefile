OBJS = main.cpp Listener.cpp Requester.cpp 
FLAGS = -g -Wall -pedantic -pthread
EXEC = node
#LIBS = 
#INCLUDE = 

all:
	g++ $(OBJS) -o $(EXEC) $(FLAGS) 


