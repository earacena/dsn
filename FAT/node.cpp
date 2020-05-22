#ifndef NODE_H
#define NODE_H

#include <vector>		//blocks vector
#include "fat.cpp"		//fat
#include "block.cpp"	//block data

class Node{
	public:
		Node(int nodeNumber);
		Node(int nodeNumber, std::string userInput);

		void pushBackBlock(Block b);

		//getters and setters
		int getNodeNumber();
		void setNodeNumber(int nodeNumber);
		std::vector<Block>& getBlocks();
		Fat getFat();

		//destructor
		~Node;
	private:
		int nodeNumber;
		std::vector<Block> blocks;
		Fat fat;
};

//constructors
Node::Node(int nodeNumber) {
	this->nodeNumber = nodeNumber;
}

Node::Node(int nodeNumber, std::string userInput) {
	this->nodeNumber = nodeNumber;
	Block block(userInput);
	this->blocks.push_back(block);
	Fat fat();
}

//functions
void Node::pushBackBlock(Block b) {
	this->blocks.push_back(b);
}

//getters and setters
int Node::getNodeNumber() {
	return nodeNumber;
}

void Node::setNodeNumber(int nodeNumber) {
	this->nodeNumber = nodeNumber;
}

std::vector<Block>& Node::getBlocks() {
	return blocks;
}

Fat Node::getFat() {
	return fat;
}

Node::~Node() {
	delete[] nodeNumber;
	blocks.clear();
	delete[] blocks;
	delete[] fat;
}
#endif
