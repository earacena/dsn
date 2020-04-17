#ifndef FAT_H
#define FAT_H

#include <iostream>		//counts
#include <vector>		//data storage
#include "block.cpp"	//block data

class Fat{
	public:
		void 
	private:
		//std::vector<node*> nodeVec;	//fat holds nodes
		std::vector<Node>* nodes;
		std::string fileName;
};	

std::vector<std::string>& Fat::getNodes() {	
	return nodes;
}

void Fat::setNodes(const std::vector<std::string>& vecToPopulate) { //pass in reference to make it faster, const so it doesnt get modified.
	this.nodes = vecToPopulate;
}

std::string Fat::getFileName() {
	return fileName;
}

void Fat::setFileName(std::string fileName) {
	this->fileName = fileName;
}

#endif 
