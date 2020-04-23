#ifndef NODE_H
#define NODE_H

#include <vector>		//data storage
#include "fat.cpp"		//fat
#include "block.cpp"		//block data
//class Fat;
class Node{
	public:
		Node(int nodeNumber);
		Node(int nodeNumber, std::string userInput);
		
		//getters and setters
		int getNodeNumber();
		void setNodeNumber(int nodeNumber);
		//Block getBlock();
		std::vector<Block>& getBlocks();
		Fat getFat();

	private:
		int nodeNumber;
		std::vector<Block> blocks;
		//Fat* fat;
		Fat fat;
};

Node::Node(int nodeNumber) {
	this->nodeNumber = nodeNumber;
}

Node::Node(int nodeNumber, std::string userInput) {
	this->nodeNumber = nodeNumber;
	Block block(userInput);
	this->blocks.push_back(block);
	Fat fat();
}

//getters and setters
int Node::getNodeNumber() {
	return nodeNumber;
}

void Node::setNodeNumber(int nodeNumber) {
	this->nodeNumber = nodeNumber;
}

/*Block Node::getBlock() {
	return this->block;
}*/

std::vector<Block>& Node::getBlocks() {	
	return blocks;
}

Fat Node::getFat() {
	return fat;
}
#endif 
