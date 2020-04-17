#ifndef BLOCK_H
#define BLOCK_H

#include <iostream> //couts

class Block{
	public:
		Block(std::string userInput)	//constructor
		std::string getData();	//getter	
		void setData();	//setter
	private:
		std::string data;
};	

Block::Block(std::string userInput) {
	this.data = userInput;	//data
}

std::string Block::getData() {
	return data;
}

void Block::setData(std::string data) {
	this->data = data;
}

#endif 
