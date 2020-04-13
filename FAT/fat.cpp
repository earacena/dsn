#ifndef FAT_H
#define FAT_H

#include <iostream> //counts
#include <vector> //data storage
#include <map>	//maps
#include "block.cpp"	//block data

//node will hold blocks - node number, and numblocks
//blockData hold the data, location, 
struct node {	//node holds node number and the blocks of information
	int nodeNumber;
	std::map<int, Block> blockInfo;	//block holds information, each block may hold a different file's information.
	//just add onto the map if theres any new blocks that needs to be added. 
	//add onto the block if theres any new information that needs to be added. 
};

class Fat{
	public:
		//constructors
		Fat(std::string userInput, int numFiles) {
			std::vector<std::string> tempVec;
			split(tempVec, userInput, numFiles);	//Splits 
			int location = 1000;

			for (int i = 0; i < numFiles; i++) {
				node* temp = new node;
				temp->nodeNumber = i;
				Block::block(tempVec[i], location + (4 * i));	//make info blocks starting at 1000 and every 4 bits.
				//temp information right now just for testing purposes
			}

		}

		//functions
		void split(std::vector<std::string>& vecToPopulate, std::string stringToSplit, int numPieces);	//split the string into numPieces pieces and send back the vector. 
		void createNode(int nodeNumber, int numBlocks);
		//void createFat();
		//void createFile(string userInput);
		
		//getters
		//int getNodeNumber();

	private:
		std::vector<node*> nodeVec;	//fat holds nodes
		//std::string fileName;
};	

void split(std::vector<std::string>& vecToPopulate, std::string stringToSplit, int numPieces) {
	int stringLength = stringToSplit.length();
	if (stringLength == 0) {
		std::cout << "Please enter proper input" << std::endl;
		return;
	}

	int splitSize = stringLength / numPieces;	
	splitSize++; //adjusts for leftover letters
	std::string temp;
	for (int i = 0; i < numPieces; i++){
		temp = stringToSplit.substr(i*splitSize,(i+1)*splitSize);
		//split size = 5 in the example
		//(0 - 5, 5-10, 10-15)
		vecToPopulate.push_back(temp);
	}
}

void Fat::createNode(int nodeNumber, int numBlocks) {
	std::cout << "creating node" << std::endl;
	//node* temp = new node;
	//temp->nodeNumber = ;
	//temp->numBlocks = ;
}

/*void Fat::createFat(int numFiles) {
	std::cout << "Setting up Fat" << std::endl;
	for (int i = 0; i < numFiles; i++) {
		ofstream file;
		file.open(std::to_string(i) + ".txt");	//makes 3 files with number names
		for (int j = 0; j < 2; j++) {	//populate file with the number 3 times. 
			file << i;	
		}
		file.close();
	}
}*/

/*void Fat::createFile(std::string userInput) {	//check fat for recent file, then add in
	std::cout << "Creating file" << std::endl;
	ofstream file;
	file.open("5.txt");
	file << userInput;
	file.close();
}
(*/
/*std::string Fat::getFileName() {
	return this.fileName;
}*/

#endif 
