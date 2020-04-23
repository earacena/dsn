#include <iostream>		//couts
#include <vector>		//data storage
#include "node.cpp"		//nodes
#include "block.cpp"	//blocks for createBlock
#include <fstream>		//file generation
#include <iomanip>		//io manipulation - for inputs 
#include <sstream>		//for importing FAT (iss >> x)
#include <stdlib.h>		//random

void split(std::vector<std::string>& vecToPopulate, std::string stringToSplit, int numPieces) {	//Splits stringToSplit into numPieces and puts them into vecToPopulate
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
			vecToPopulate.push_back(temp);
			temp = "";
		}
	}

	if (vecToPopulate.size() != numPieces) {	//get last string if not perfect division
		vecToPopulate.push_back(temp);
	}
}	  
void shuffleVector(std::vector<std::string> vecToShuffle) {
	int random;
	for (int i = 0; i < vecToShuffle; i++) {
		random = rand() % vecToShuffle;	//between 0 and size()-1
		swapStrings(vecToShuffle[i], vecToShuffle[random]);
	}
}
void swapStrings(std::string& a, std::string& b) {
	std::string temp = a;
	a = b;
	b = temp;
}
void createFatFromVector(std::vector<Node> &nodes, std::vector<std::string> &splitStrings) {	//populates fat with splitStrings
	for (int i = 0; i < splitStrings.size(); i++) {
		Node temp(nodes.size(), splitStrings[i]);	//nodeNumber, content 
		nodes.push_back(temp);
	}
	splitStrings.clear();
}
void createBlocks(std::vector<Node> &nodes, std::string content){
	Block temp(content);
	nodes[0].getBlocks().push_back(temp);
}
void printFat(std::vector<Node> &nodes){	//properly working
	std::cout << "Showing File Allocation Table" << std::endl;

	if(nodes.size() > 0){
		for(int i=0; i<nodes.size(); i++){	//goes through each node
			for (int j = 0; j < nodes[i].getBlocks().size(); j++) {	//goes through each block of that node.
				std::cout << "Node number: " << nodes[i].getNodeNumber() << " content: " << nodes[i].getBlocks()[j].getData() << std::endl;
			}
		}
	}
	else{
		std::cout << "Your File Allocation Table is empty" << std::endl;
	}
}
void importFat(std::vector<Node> &nodes, std::string filename){	//need to be fixed later to adapt to new block vector.
	std::cout << "Importing File Allocation Table from file" << std::endl;
	std::ifstream myFile;
	std::string line, content;
	int nodeNumber, blockNumber, counter=0;

	myFile.open(filename);
	if (myFile) {
		while (getline(myFile, line)) {
			istringstream iss(line);
			iss >> nodeNumber;
			Node temp(nodeNumber);
			nodes.push_back(temp);
			while (iss >> blockNumber >> content) {
				Block temp(content);
				nodes[counter].getBlocks().push_back(temp);
			}
			counter++;
		}
		myFile.close();
	}
	else {
		std::cout << "error opening file" << std::endl;
		return;
	}
}
void exportFat(std::vector<Node> &nodes){	//working properly
	std::cout << "Exporting File Allocation Table as backup.txt" << std::endl;

	std::ofstream myFile;

	myFile.open("backup.txt");
	//similar to the print function, just outputs.
	for (int i = 0; i < nodes.size(); i++) {
		for (int j = 0; j < nodes[i].getBlocks().size(); j++) {
			myFile << nodes[i].getNodeNumber() << " " << nodes[i].getBlocks()[j].getData() << std::endl;
		}
	}
	myFile.close();
}

int main (int argc, char *argv[]){
	int userChoice, numFiles;
	std::string userInput;
	bool fatIsSetUp = false;
	std::vector<std::string> splitStrings;
	std::vector<Node> nodes;

	while (true) 
	{
		std::cout << std::left	<< std::setw(15) << "1: Set up FAT" 
								<< std::setw(15) << "2: Add new file" 
								<< std::setw(15) << "3: Show FAT" 
								<< std::setw(15) << "7: Clear FAT" 
								<< std::setw(15) << "8: Import FAT" 
								<< std::setw(15) << "9: Export FAT" << std::endl;
		std::cin >> userChoice;

		switch (userChoice) 
		{
			case 1:	//set up fat once: makes the nodes and keeps that amount of nodes until deleted.
				if(fatIsSetUp == false){
					std::cout << "Setting up File Allocation Table" << std::endl;
					/*std::cout << "Enter input to store" << std::endl;
					std::cin >> userInput;
					std::cout << "enter amount of files: " << std::endl; 
					std::cin >> numFiles;*/
					userInput = "Hello there, i am a very long string";
					numFiles = 3;

					split(splitStrings, userInput, numFiles); //string vector, string, int					
					createFatFromVector(nodes, splitStrings);
					fatIsSetUp = true;
				}			
				else{
					std::cout << "The File Allocation Table is already set up" << std::endl;
				}	
				break;
			case 2:	//any additional file will be split and added randomly to different nodes that was already set up.
				if (fatIsSetUp == true) {	//only adds these blocks if there are nodes to hold them.
					//std::cout << "Enter what to put into the file" << std::endl;
					//std::cin >> userInput;
					userInput = "This is my plain text that wil' be split up";

					split(splitStrings, userInput, numFiles);
					for(int i=0; i<splitStrings.size(); i++){
						createBlocks(nodes, splitStrings[i]);
					}
					splitStrings.clear();
				}
				else {
					std::cout << "Please set up the File Allocation Table First" << std::endl;
				}
				break;
			case 3:
				printFat(nodes);
				break;
			case 7: 
				std::cout << "Clearing File Allocation Table" << std::endl;
				nodes.clear();
				fatIsSetUp = false;
				break;
			case 8: 
				//std::cout << "Enter file name for import (add extension if applies)" << std::endl;
				//std::cin >> userInput
				userInput = "backup.txt";

				importFat(nodes, userInput);
				break;
			case 9:
				exportFat(nodes);
				break;
			default:
				std::cout << "You didn't enter a correct command" << std::endl;
				break;
		}
	}
	return 0;
}
