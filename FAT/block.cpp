#ifndef BLOCK_H
#define BLOCK_H

#include <iostream> //couts
#include "node.cpp"

//class Block : public Node{
class Block{
	public:
		Block();
		Block(std::string userInput);	//constructor
		//Node(int nodeNumber) : Block(int nodeNumber);

		//getters and setters
		std::string getData();		
		void setData(std::string data);	
	private:
		std::string data;
};	

Block::Block(){
	this->data = "bleh";
}

Block::Block(std::string userInput) {
	this->data = userInput;	//data
}

/*Block::Block(int nodeNumber) : Node::Node(int nodeNumber){
	this->nodeNumber = nodeNumber;
}*/

//getters and setters
std::string Block::getData() {
	return data;
}

void Block::setData(std::string data) {
	this->data = data;
}

#endif 
