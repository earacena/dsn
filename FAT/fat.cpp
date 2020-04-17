#ifndef FAT_H
#define FAT_H

#include <iostream>		//counts
#include <vector>		//data storage
#include "block.cpp"	//block data
#include "node.cpp"	//nodes

class Fat{
	public:
		std::vector<std::string>& getNodes();
		void setNodes(const std::vector<Node>& vector);
		std::string getFileName();
		void setFileName(std::string fileName);
	private:
		//std::vector<node*> nodeVec;	//fat holds nodes
		std::vector<Node>* nodes;
		std::string fileName;
};	

std::vector<std::string>& Fat::getNodes() {	
	return nodes;
}

void Fat::setNodes(const std::vector<Node>& vecToPopulate) { //pass in reference to make it faster, const so it doesnt get modified.
	this->nodes = vecToPopulate;
}

std::string Fat::getFileName() {
	return fileName;
}

void Fat::setFileName(std::string fileName) {
	this->fileName = fileName;
}

#endif 
