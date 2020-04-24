#ifndef MAP_H
#define MAP_H

#include <vector>		//blocks vector
//#include "fat.cpp"		//fat
//#include "block.cpp"	//block data

class Map{
	public:
		Map(int nodeNumber, std::string userInput);

		void makePair(Block b);

		//getters and setters
		std::string getFirst();
		void setFirst(std::string s);
		std::string getSecond();
		void setSecond(std::string s);

	private:
		std::string first, second;
		//have a vector that would hold these????
};

Map::Map(std::string first, std::string second) {
	this->first = first;
	this->second = second;
}

//functions
void Node::makePair(std::string first, std::string second) {
	//this->blocks.push_back(b);
}

//getters and setters
std::string Map::getFirst() {
	return first;
}

void Map::setFirst(std::string s) {
	this->first = s;
}

std::string Map::getSecond() {
	return second;
}

void Map::setSecond(std::string s) {
	this->second = s;
}
#endif
