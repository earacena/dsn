OBJS = main.cpp 
FLAGS = -g -Wall -pedantic -pthread
EXEC = node
#LIBS = 
#INCLUDE = 

all:
	g++ $(OBJS) -o $(EXEC) $(FLAGS) 


