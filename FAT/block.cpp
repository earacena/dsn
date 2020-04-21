#ifndef BLOCK_H
#define BLOCK_H

#include <iostream> //couts

class Block{
	public:
		Block(std::string userInput, int nodeNumber);	//constructor

		//getters and setters
		std::string getData();		
		void setData(std::string data);	
		int getNodeNumber();
		void setNodeNumber(int nodeNumber);
	private:
		std::string data;
		int nodeNumber;
};	

Block::Block(std::string userInput, int nodeNumber) {
	this->data = userInput;	//data
	this->nodeNumber = nodeNumber;
}

//getters and setters
std::string Block::getData() {
	return data;
}

void Block::setData(std::string data) {
	this->data = data;
}

int Block::getNodeNumber() {
	return nodeNumber;
}

void Block::setNodeNumber(int nodeNumber) {
	this->nodeNumber = nodeNumber;
}

#endif 
