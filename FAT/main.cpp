#include <iostream> //couts
#include <vector> //data storage
#include "fat.cpp"	//include FAT
#include "node.cpp"	//nodes
#include <fstream>	//file generation

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
	stringToSplit.clear();
}	  
void createFatFromVector(std::vector<Node> &nodes, std::vector<std::string> splitStrings) {	//populates fat with splitStrings
	for (int i = 0; i < splitStrings.size(); i++) {
		Node temp(nodes.size(), splitStrings[i]);	//nodeNumber, content 
		nodes.push_back(temp);
	}
}
void createFatFromInput(std::vector<Node> &nodes, int nodeNumber, std::string content) {	//for importing fats: creates each input individually.
	Node temp(nodeNumber, content);
	nodes.push_back(temp);
}
void printFat(std::vector<Node> &nodes){
	std::cout << "Showing File Allocation Table" << std::endl;

	if(nodes.size() > 0){
		for(int i=0; i<nodes.size(); i++){	//goes through each node
			for (int j = 0; j < nodes[i].getBlocks().size(); i++) {	//goes through each block of that node.
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

	int nodeNumber;
	std::string content;
	std::ifstream myFile;

	myFile.open(filename);
	while (!myFile.eof()) {
		myFile >> nodeNumber >> content;
		//once map is implemented
		//myFile >> nodeNumber >> blockNumber >> content;
		createFatFromInput(nodes, nodeNumber, content);
	}
	myFile.close();
}
void exportFat(std::vector<Node> &nodes){
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
	int userChoice;
	std::string userInput;
	int numFiles;
	std::vector<std::string> splitStrings;
	std::vector<Node> nodes;

	while (true) 
	{
		std::cout << "Enter 1 to set up Fat, \n 2 to create new file, \n 3 to show fat, \n 7 to clear fat, \n 8 to import fat, \n 9 to export fat" << std::endl;
		std::cin >> userChoice;

		switch (userChoice) 
		{
			case 1:	//set up fat once.
				std::cout << "Setting up File Allocation Table" << std::endl;
				/*std::cout << "Enter input to store" << std::endl;
				std::cin >> userInput;
				std::cout << "enter amount of files: " << std::endl; 
				std::cin >> numFiles;*/
				userInput = "Hello there, i am a very long string";
				numFiles = 3;

				split(splitStrings, userInput, numFiles); //string vector, string, int					
				createFatFromVector(nodes, splitStrings);
				break;
			case 2:
				//std::cout << "Enter what to put into the file" << std::endl;
				//std::cin >> userInput;
				userInput = "This is my plain text that wil' be split up";
				numFiles = 3;

				split(splitStrings, userInput, numFiles);
				//do code
				break;
			case 3:
				printFat(nodes);
				break;
			case 7: 
				std::cout << "Clearing File Allocation Table" << std::endl;
				nodes.clear();
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
