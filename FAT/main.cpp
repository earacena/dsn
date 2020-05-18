#include <iostream>		//couts
#include <vector>		//data storage
#include "node.cpp"		//nodes
// #include "block.cpp"	//blocks for createBlock
#include <fstream>		//file generation
#include <iomanip>	    //io manipulation - for inputs
#include <sstream>		//for importing FAT (iss >> x)
#include <stdlib.h>		//rand
#include <map>			//map multi map
#include "Blockchain/blockchain.hpp"
// #include "Blockchain/blocks.hpp"

#define RESET 	"\033[0m"
#define YELLOW 	"\033[33m"
#define RED 	"\033[31m"
#define CYAN 	"\033[36m"

//nice functions
void printNice(std::string print){
	std::cout << YELLOW << "*****" << print << "*****"  << RESET << std::endl;
}
void populateNameVecWithRandom(std::vector<std::string> &fileNames, std::vector<std::string> &content) {
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

	content.push_back("We're no strangers to love");
	content.push_back("You know the rules and so do I");
	content.push_back("A full commitment's what I'm thinking of");
	content.push_back("You wouldn't get this from any other guy");
	content.push_back("I just wanna tell you how I'm feeling");
	content.push_back("Gotta make you understand");
	content.push_back("Never gonna give you up");
	content.push_back("Never gonna let you down");
	content.push_back("Never gonna run around and desert you");
	content.push_back("Never gonna make you cry");
	content.push_back("Never gonna say goodbye");
	content.push_back("Never gonna tell a lie and hurt you");
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
void split(std::vector<std::string> &vecToPopulate, std::string stringToSplit, int numPieces) {	//Splits stringToSplit into numPieces and puts them into vecToPopulate
	int length = stringToSplit.length();
	if (length == 0) {
		printNice("Nothing to Split");
		return;
	}

	int splitSize = length / numPieces;
	splitSize++;
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
//importing and exporting
void importFat(std::vector<Node> &nodes, std::string filename){	//not working
	printNice("Importing File Allocation Table from file");
	std::ifstream myFile;
	std::string line, fileName;
	int nodeNumber, blockNumber, counter = 0;

	//node number, filename block filename block filename block
	myFile.open(filename);
	if (myFile) {
		while (getline(myFile, line)) {
			std::istringstream iss(line);
			iss >> nodeNumber;

			Node temp(nodeNumber);
			nodes.push_back(temp);
			while (iss >> blockNumber >> fileName) {
				Block b(fileName);
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
	for (int i = 0; i < nodes.size(); i++) {	//prints node number, file name, then block number
		myFile << nodes[i].getNodeNumber() << " ";
		for (int j = 0; j < nodes[i].getBlocks().size(); j++) {
			myFile << nodes[i].getBlocks()[j].getFileName() << " " << j << " "; //<< nodes[i].getBlocks()[j].getData() << " ";
		}
		myFile << std::endl;
	}
	myFile.close();
}
void exportBlock(std::string fileName, std::string content) {
	const std::string path = "storage/";
	std::ofstream temp(path+fileName);
	temp << content;
	temp.close();
}

//populate Fat
void createBlocks(std::vector<Node> &nodes, std::multimap<std::string, std::pair<int, int>> &table, std::vector<std::string> &splitStrings, std::string fileName, Blockchain &blockchain){
	//block shuffling - the order of temp is where the strings in splitStrings will go
	std::vector<int> temp;
	int counter = 0;
	for (int i = 0; i < splitStrings.size(); i++) {
		temp.push_back(i);
		counter++;
	}
	shuffleVector(temp);

	for (int i = 0; i < splitStrings.size(); i++) {
		//block generation and fat population
		table.insert(std::make_pair(fileName, std::make_pair(temp[i], nodes[temp[i]].getBlocks().size()))); //fileName, pair(nodeNumber, blockNumber)
		//Block b(fileName);
		Block b(fileName, splitStrings[i]);
		nodes[temp[i]].pushBackBlock(b);

		TransactionData data;
		data.fileName = fileName;
		data.content = splitStrings[i];
		data.receiverNode = temp[i];
		data.nodeBlock = nodes[temp[i]].getBlocks().size()-1;
		data.timestamp = time(&data.timestamp);
		// adding each transactional data to the blockchain
		blockchain.addBlock(data); // Data of each transaction

		//file generation (for emmanuel)
		std::string newFileName = fileName + "_" + std::to_string(temp[i]) + "_" + std::to_string(nodes[temp[i]].getBlocks().size()-1);   //filename_nodenumber_blocknumber
		exportBlock(newFileName, splitStrings[i]);	//filename, content
	}

	splitStrings.clear();
}
/*void createBlocksFromFiles(std::vector<Node>& nodes, std::multimap<std::string, std::pair<int, int>>& table, std::string fileName) {	//work in progress

}*/
void deleteBlock(std::vector<Node>& nodes, std::multimap<std::string, std::pair<int, int>>& table, int userInput) {	//in progress, create destructors
	nodes[userInput].getBlocks().clear();
	for (int i = 0; i < nodes.size(); i++) {
		//if(nodes[i].getBlocks());
	}
}
//print
void printFatContent(std::vector<Node> &nodes){	//properly working
	if(nodes.size() > 0){
		printNice("Showing File Allocation Table");
		for(int i=0; i<nodes.size(); i++){	//goes through each node
			std::cout << "Node number " << nodes[i].getNodeNumber() << std::endl;;
			for (int j = 0; j < nodes[i].getBlocks().size(); j++) {	//goes through each block of that node.
				std::cout << std::setw(5) << " " << "File name: " << std::left << std::setw(10) << nodes[i].getBlocks()[j].getFileName() << std::endl; //<< " content: " << std::left << std::setw(20) << nodes[i].getBlocks()[j].getData() << std::endl;
			}
		}
	}
	else{
		printNice("Please set up the File Allocation Table first");
	}
}
void printMap(std::multimap<std::string, std::pair<int, int>> &table) {
    if(!table.empty()){
        printNice("Showing file table");
        for (auto i = table.begin(); i != table.end(); i++) {	//prints whole map
            std::cout << "File name: " << std::left << std::setw(20) << i->first << " is located in Node " << std::setw(5) << i->second.first << " block " << std::setw(5) << i->second.second << std::endl;
        }
    }
    else{
        printNice("Please set up the File Allocation Table first");
    }
}
void printCombinedFile(std::vector<Node> &nodes, std::multimap<std::string, std::pair<int, int>> &table, std::string userInput, int numFiles){
    std::multimap<std::string, std::pair<int, int>>::iterator it = table.find(userInput);
    std::string s;
    int nodeNumber, blockNumber;
    for(int i=0;i<numFiles;i++){
        nodeNumber = it->second.first;
        blockNumber =  it->second.second;
        //std::cout << "node number: " << nodeNumber << " block number: " << blockNumber << " data: " << nodes[nodeNumber].getBlocks()[blockNumber].getData() << std::endl;
        s += nodes[it->second.first].getBlocks()[it->second.second].getData();
        it++;
    }
    std::cout << "The combined file " << userInput << " is: " << s << std::endl;
}
void printEverything(std::vector<Node> &nodes) {
    if(nodes.size() > 0){
		printNice("Showing File Allocation Table");
		for(int i=0; i<nodes.size(); i++){	//goes through each node
			std::cout << "Node number " << nodes[i].getNodeNumber() << std::endl;;
			for (int j = 0; j < nodes[i].getBlocks().size(); j++) {	//goes through each block of that node.
				std::cout << std::setw(5) << " " << "File name: " << std::left << std::setw(10)  << nodes[i].getBlocks()[j].getFileName() << " content: " << std::left << std::setw(20) << nodes[i].getBlocks()[j].getData() << std::endl;
			}
		}
	}
	else{
		printNice("Please set up the File Allocation Table first");
	}
}

int main (int argc, char *argv[]){
	int userChoice, numFiles;
	std::string userInput, fileName;
	bool fatIsSetUp = false;
	std::vector<std::string> splitStrings;
	std::vector<Node> nodes;
	std::multimap<std::string, std::pair<int, int>> table;


	//filenames
	int nameIterator = 0;	//iterates every use
	std::vector<std::string> fileNames;
	std::vector<std::string> fileContent;
	populateNameVecWithRandom(fileNames, fileContent);

	// blockchain: @params: difficulty of PoW algorithm. Higher the more difficulty.
	Blockchain bchain(2); 

	while (true)
	{
		std::cout << CYAN << "1: Set up FAT, 2: Add file, 3: Print Fat, 4: Print File Names, 5: Print Everything, 6: Delete Block 7: Clear FAT, 8: Import FAT, 9: Export FAT, 10: Search File, 11: Print Blockchain, 12: Is it a valid Blockchain?" << RESET << std::endl;
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
					createBlocks(nodes, table, splitStrings, fileName, bchain);
					nameIterator++;
				}
				else {
					printNice("Please set up the File Allocation Table first");
				}
				break;
			case 3:
				printFatContent(nodes);
				break;
			case 4:
				printMap(table);
				break;
            case 5:
				printEverything(nodes);
                break;
			case 6:
				printNice("Enter the name of the file you want to delete");
				std::cin >> userChoice;
				deleteBlock(nodes, table, userChoice);
				break;
			case 7:
				printNice("Clearing File Allocation Table");
				nodes.clear();
				table.clear();
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
				exportFat(nodes);
				break;
			case 10:
				if (nodes.size() > 0) {
					while (true) {
						printNice("Enter the name of the file you want to search for, and q to stop");
						std::cin >> userInput;
						if (userInput != "q") {
							printCombinedFile(nodes, table, userInput, numFiles);
						}
						else {
							printNice("Quitting command");
							break;
						}
					}
				}
				else {
					printNice("Please set up the File Allocation Table first");
				}
				break;
			case 11:
				printNice("Showing Blockchain");
				bchain.printChain();
				break;
			case 12:
				if (bchain.isChainValid()) {
					printNice("VALID");
				}
				else {
					printNice("INVALID");
				}
				break;
			default:
				std::cout << RED << "*****You didn't enter a proper command*****" << RESET << std::endl;
				break;
		}
	}
	return 0;
}
