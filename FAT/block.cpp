#ifndef BLOCK_H
#define BLOCK_H

#include <iostream> //counts

template <typename T>
class Block{
	public:
		Block(T userInput, int location){
			this.data = userInput;	//data
			this.location = location;	//location of block
		}
		T getData();
		int getLocation();
	private:
		T data;
		int location;
};	

template <typename T>
T Block::getData() {
	return this.data;
}

int Block::getLocation() {
	return this.location;
}

template <typename T>
void setData(T data) {
	this.data = data;
}

void setLocation(int location) {
	this.location = location;
}
#endif 
