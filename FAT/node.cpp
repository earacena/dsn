#ifndef NODE_H
#define NODE_H

#include <iostream>		//couts
#include <vector>		//data storage
#include <map>			//maps
#include "block.cpp"	//block data
#include "fat.cpp"		//fat

class Node{
	public:
		Node(std::string userInput, int nodeNumber);

		void split(std::vector<std::string>& vecToPopulate, std::string stringToSplit, int numPieces);	//split string into vector 
		void createFat(std::vector<Node> &nodes, std::string userInput, int numFiles);	//create nodes
		int getNodeNumber();
		void setNodeNumber(int nodeNumber);

	private:
		int nodeNumber;
		Block block;	//blocks currently just hold 1 piece of data. Later implement to hold more. 
		//std::map<int, Block> blockInfo;	//use case 4, when each block more than 1 piece of information. Do later
		Fat fat;
};	

Node::Node(std::string userInput, int nodeNumber) {
	Block block(userInput);
	this->block = block;
	this->nodeNumber = nodeNumber;
}

void Node::split(std::vector<std::string>& vecToPopulate, std::string stringToSplit, int numPieces) {	//works completely, maybe check out cases where length < numPieces
	int length = stringToSplit.length();
	if (length == 0) {	//check empty
		std::cout << "Nothing to Split" << std::endl;
		return;
	}

	int splitSize = length / numPieces;
	std::string temp;

	for (int i = 0; i < length; i++) {	//split
		temp += stringToSplit[i];
		if (i % splitSize == 0 && i != 0) {
			//std::cout << "I have Split " << i << " % " << splitSize << std::endl;
			vecToPopulate.push_back(temp);
		}
	}

	if (vecToPopulate.size() != numPieces) {	//get last string if not perfect division
		vecToPopulate.push_back(temp);
	}
}

void Node::createFat(std::vector<Node> &nodes, std::string userInput, int numFiles) {
	std::vector<std::string> splitStrings;
	split(splitStrings, userInput, numFiles);

	for (int i = 0; i < splitStrings.size(); i++) {
		Node temp(splitStrings[i], nodes.size());
		//fat();
		nodes.push_back(temp);
	}
}


/*for (int i = 0; i < numFiles; i++) {

	node* temp = new node;
	temp->nodeNumber = i;
	Block::block(tempVec[i], location + (4 * i));	//make info blocks starting at 1000 and every 4 bits.
	//temp information right now just for testing purposes
}*/








//getters and setters
int Node::getNodeNumber() {
	return nodeNumber;
}

void Node::setNodeNumber(int nodeNumber) {
	this.nodeNumber = nodeNumber;
}

/*Block Node::getBlock() {
	return block;
}*/

/*void Node::setBlock(string s) {

}*/

/*Fat Node::getFat() {
	return fat;
}*/

/*void Node::setFat(){

}*/
#endif 
