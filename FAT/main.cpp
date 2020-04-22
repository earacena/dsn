#include <iostream> //couts
#include <vector> //data storage
#include "fat.cpp"	//include FAT
#include "node.cpp"	//nodes
#include <fstream>	//file generation

//Splits stringToSplit into numPieces and puts them into vecToPopulate
void split(std::vector<std::string>& vecToPopulate, std::string stringToSplit, int numPieces) {	//works completely, maybe check out cases where length < numPieces
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
}	 //works 100%
void createFatFromVector(std::vector<Node> &nodes, std::vector<std::string> splitStrings) {	//vector to nodes
	for (int i = 0; i < splitStrings.size(); i++) {
		Node temp(nodes.size(), splitStrings[i]);	//string, nodeNumber
		nodes.push_back(temp);
	}
}
void createFatFromInput(std::vector<Node> &nodes, int nodeNumber, std::string content) {	
	Node temp(nodeNumber, content);
	nodes.push_back(temp);
}
void printFat(std::vector<Node> &nodes){
	for(int i=0; i<nodes.size(); i++){
		std::cout << "Node number: " << nodes[i].getNodeNumber() << " content: " << nodes[i].getBlock().getData() << std::endl;
	}
}
void importFat(std::vector<Node> &nodes, std::string filename){
	int nodeNumber;
	std::string content;
	std::ifstream myFile;

	myFile.open(filename);
	while (!myFile.eof) {
		myFile >> nodeNumber >> content;
		//once map is implemented
		//myFile >> nodeNumber >> blockNumber >> content;
		createFatFromInput(nodes, nodeNumber, content);
	}
	myFile.close();
}
void exportFat(std::vector<Node> &nodes){
	std::ofstream myFile;

	myFile.open("backup.txt");
	//similar to the print function, just outputs.
	for (int i = 0; i < nodes.size(); i++) {
		myFile << nodes[i].getNodeNumber() << " " << nodes[i].getBlock().getData() << std::endl;
	}
	myFile.close();
}

int main (int argc, char *argv[]){
	int userChoice;
	std::string userInput;
	int numFiles;
	bool fatIsSetUp = false;
	std::vector<std::string> splitStrings;
	std::vector<Node> nodes;

	while (true) 
	{
		std::cout << "Enter 1 to set up Fat, 2 to create new file, 3 to show fat, 7 to clear fat, 8 to import fat, 9 to export fat" << std::endl;
		std::cin >> userChoice;

		switch (userChoice) 
		{
			case 1:	//set up fat once.
				if (fatIsSetUp == false) {
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
					std::cout << "Fat already set up, please enter another comand" << std::endl;
				}
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
				std::cout << "Showing File Allocation Table" << std::endl;

				printFat(nodes);
				break;
			case 7: 
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
