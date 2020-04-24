#include <iostream>		//couts
#include <vector>		//data storage
#include "node.cpp"		//nodes
#include "block.cpp"	//blocks for createBlock
#include <fstream>		//file generation
#include <iomanip>	    //io manipulation - for inputs
#include <sstream>		//for importing FAT (iss >> x)
#include <stdlib.h>		//rand
#include <map>			//map multi map

#define RESET 	"\033[0m"
#define YELLOW 	"\033[33m"
#define RED 	"\033[31m"
#define CYAN 	"\033[36m"

//nice functions 
void printNice(std::string print){
	std::cout << YELLOW << "*****" << print << "*****"  << RESET << std::endl;
}
void populateNameVecWithRandom(std::vector<std::string>& fileNames, std::vector<std::string>& content) {
	fileNames.push_back("one");
	fileNames.push_back("two");
	fileNames.push_back("three");
	fileNames.push_back("four");
	fileNames.push_back("five");
	fileNames.push_back("six");
	fileNames.push_back("seven");
	fileNames.push_back("eight");
	fileNames.push_back("nine");
	fileNames.push_back("ten");
	fileNames.push_back("eleven");
	fileNames.push_back("twelve");

	content.push_back("1 We're no strangers to love");
	content.push_back("2 You know the rules and so do I");
	content.push_back("3 A full commitment's what I'm thinking of");
	content.push_back("4 You wouldn't get this from any other guy");
	content.push_back("5 I just wanna tell you how I'm feeling");
	content.push_back("6 Gotta make you understand");
	content.push_back("7 Never gonna give you up");
	content.push_back("8 Never gonna let you down");
	content.push_back("9 Never gonna run around and desert you");
	content.push_back("10 Never gonna make you cry");
	content.push_back("11 Never gonna say goodbye");
	content.push_back("12 Never gonna tell a lie and hurt you");
}   
template <class T> void swap(T &a, T &b) {	//works
	T temp = a;
	a = b;
	b = temp;
}
template <class T> void shuffleVector(std::vector<T> &vecToShuffle) {
	int random;
	for (int i = 0; i < vecToShuffle.size(); i++) {
		random = rand() % vecToShuffle.size();	//between 0 and size()-1
		swap(vecToShuffle[i], vecToShuffle[random]);
	}
}
void split(std::vector<std::string>& vecToPopulate, std::string stringToSplit, int numPieces) {	//Splits stringToSplit into numPieces and puts them into vecToPopulate
	int length = stringToSplit.length();
	if (length == 0) {	
		printNice("Nothing to Split");
		return;
	}

	int splitSize = length / numPieces;
	std::string temp;

	for (int i = 0; i < length; i++) {		//split
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
//populate Fat
void createBlocks(std::vector<Node> &nodes, std::multimap<std::string, pair<int, int>> table, std::vector<std::string> &splitStrings, std::string fileName){
	std::vector<int> temp;	//the order of temp is where the blocks in splitStrings will go
	int counter = 0;
	for (int i = 0; i < splitStrings.size(); i++) {
		temp.push_back(i);
		counter++;
	}
	shuffleVector(temp);

	for (int i = 0; i < splitStrings.size(); i++) {
		Block b(splitStrings[i], fileName);
		nodes[temp[i]].pushBackBlock(b);
		table.insert(std::make_pair(fileName, std::make_pair(temp[i], nodes[temp[i]].getBlocks().size()))); //fileName, pair(nodeNumber, blockNumber)
	}
	splitStrings.clear();
}
//print
void printFatContent(std::vector<Node> &nodes){	//properly working
	if(nodes.size() > 0){
		printNice("Showing File Allocation Table");
		int space = 20;
		for(int i=0; i<nodes.size(); i++){	//goes through each node
			std::cout << std::left << "Node number " << nodes[i].getNodeNumber() << std::endl;;
			for (int j = 0; j < nodes[i].getBlocks().size(); j++) {	//goes through each block of that node.
				std::cout << std::setw(space) << " " << "File name: " << nodes[i].getBlocks()[j].getFileName() << std::setw(space) << " content: " <<  nodes[i].getBlocks()[j].getData() << std::endl;
				//std::cout << std::setw(space) << " " << "file name: " << nodes[i].getFat().getNodes()[i].getFileName() <<" content: " << nodes[i].getBlocks()[j].getData() << std::endl;
			}
		}
	}
	else{
		printNice("Your File Allocation Table is empty");
	}
}
void printMap(std::multimap<std::string, pair<int, int>> table) {
	for (auto i = table.begin(); i != table.end(); i++) {	//prints whole map
		cout << i->first << "-----" << i->second->first << "-----" i->second->second << endl;
	}
}
//importing and exporting
void importFat(std::vector<Node> &nodes, std::string filename){	//not working
	printNice("Importing File Allocation Table from file");
	std::ifstream myFile;
	std::string line, content;
	int nodeNumber, blockNumber, counter=0;

	myFile.open(filename);
	if (myFile) {
		while (getline(myFile, line)) {
			std::istringstream iss(line);
			iss >> nodeNumber;
			Node temp(nodeNumber);
			nodes.push_back(temp);
			while (iss >> blockNumber >> content) {
				Block b(content);
				nodes[counter].getBlocks().push_back(b);
			}
			counter++;
		}
		myFile.close();
	}
	else {
		printNice("error opening file");
		return;
	}
}
void exportFat(std::vector<Node> &nodes){
	printNice("Exporting File Allocation Table as backup.txt");

	std::ofstream myFile;

	myFile.open("backup.txt");
	//similar to the print function, just outputs.
	for (int i = 0; i < nodes.size(); i++) {
		myFile << nodes[i].getNodeNumber() << " ";
		for (int j = 0; j < nodes[i].getBlocks().size(); j++) {
			myFile << nodes[i].getBlocks()[j].getFileName() << " " << nodes[i].getBlocks()[j].getData() << " ";
		}
		myFile << std::endl;
	}
	myFile.close();
}
void exportFatEasy(std::vector<Node> &nodes){
	printNice("Exporting File Allocation Table as backup.txt");

	std::ofstream myFile;

	myFile.open("backup.txt");
	//similar to the print function, just outputs.
	for (int i = 0; i < nodes.size(); i++) {
		for (int j = 0; j < nodes[i].getBlocks().size(); j++) {
			myFile << nodes[i].getNodeNumber() << " " << nodes[i].getBlocks()[j].getFileName() << " " << nodes[i].getBlocks()[j].getData() << " " << std::endl;
		}
	}
	myFile.close();
}

int main (int argc, char *argv[]){
	int userChoice, numFiles;
	std::string userInput, fileName;
	bool fatIsSetUp = false;
	std::vector<std::string> splitStrings;
	std::vector<Node> nodes;
	std::multimap<std::string, pair<int, int>> table;


	//filenames
	int nameIterator = 0;	//iterates every use
	std::vector<std::string> fileNames;
	std::vector<std::string> fileContent;
	populateNameVecWithRandom(fileNames, fileContent);

	while (true)
	{
		std::cout << CYAN << "1: Set up FAT, 2: Add file, 3: Print Fat, 4: Print File Names, 5: Search file, 7: Clear FAT, 8: Import FAT, 9: Export FAT" << RESET << std::endl;
		std::cout << "Your Input: ";
		std::cin >> userChoice;

		switch (userChoice)
		{
			case 1:	//set up fat once: makes the nodes and keeps that amount of nodes until deleted.
				if(fatIsSetUp){
					printNice("The File Allocation Table is already set up");
				}
				else{
					printNice("Setting up File Allocation Table");
					/* std::cout << "Enter amount of files: " << std::endl;
					std::cin >> numFiles;*/
					numFiles = 3;

					for (int i = 0; i < numFiles; i++) {
						Node temp(nodes.size());
						nodes.push_back(temp);
					}
					fatIsSetUp = true;
				}
				break;
			case 2:	//any additional file will be split and added randomly to different nodes that was already set up.
				if (fatIsSetUp) {	//only adds these blocks if there are nodes to hold them.
					printNice("Creating a file");
					/*std::cout << "Enter what to put into the file" << std::endl;
					std::cin >> userInput;
					std::cout << "Enter name for the file" << std::endl;
					std::cin >> fileName*/

					userInput = fileContent[nameIterator];
					fileName = fileNames[nameIterator];

					split(splitStrings, userInput, numFiles);
					createBlocks(nodes, table, splitStrings, fileName);
					nameIterator++;
				}
				else {
					printNice("Please set up the File Allocation Table First");
				}
				break;
			case 3:
				printFatContent(nodes);
				break;
			case 4:
				printMap(nodes);
				break;
			case 7:
				printNice("Clearing File Allocation Table");
				nodes.clear();
				fatIsSetUp = false;
				nameIterator = 0;
				break;
			case 8:
				//std::cout << "Enter file name for import (add extension if applies)" << std::endl;
				//std::cin >> userInput
				userInput = "backup.txt";

				if(fatIsSetUp){
					printNice("You can't import, FAT is already set up");
				}
				else{
					importFat(nodes, userInput);
					fatIsSetUp = true;
				}
				break;
			case 9:
				exportFatEasy(nodes);
				break;
			default:
				std::cout << RED << "*****You didn't enter a correct command*****" << RESET << std::endl;
				break;
		}
	}
	return 0;
}
