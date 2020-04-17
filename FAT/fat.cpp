#ifndef FAT_H
#define FAT_H

#include <iostream>		//counts
#include <vector>		//data storage
#include "node.cpp"	//nodes
#include "block.cpp"	//block data

class Fat{
	public:
		std::vector<Node>& getNodes();
		void setNodes(std::vector<Node> vector);
		std::string getFileName();
		void setFileName(std::string fileName);
	private:
		//std::vector<node*> nodeVec;	//fat holds nodes
		std::vector<Node> nodes;
		std::string fileName;
};	

std::vector<Node>& Fat::getNodes() {	
	return nodes;
}

void Fat::setNodes(std::vector<Node> vector) { //pass in reference to make it faster, const so it doesnt get modified.
	this->nodes = vector;
}

std::string Fat::getFileName() {
	return fileName;
}

void Fat::setFileName(std::string fileName) {
	this->fileName = fileName;
}

#endif 
