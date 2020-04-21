#include <iostream> //couts
#include <vector> //data storage
#include "fat.cpp"	//include FAT
#include "node.cpp"	//nodes

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
			//std::cout << "I have Split " << i << " % " << splitSize << std::endl;
			vecToPopulate.push_back(temp);
		}
	}

	if (vecToPopulate.size() != numPieces) {	//get last string if not perfect division
		vecToPopulate.push_back(temp);
	}
}

void createFat(std::vector<Node> &nodes, std::vector<std::string> splitStrings) {	//vector to nodes
	for (int i = 0; i < splitStrings.size(); i++) {
		Node temp(splitStrings[i], nodes.size());
		nodes.push_back(temp);
	}
}

void printFat(std::vector<Node> &nodes){
	for(int i=0; i<nodes.size(); i++){
		std::cout << "Node number: " << nodes[i].getNodeNumber() << " content: " << nodes[i].getBlock().getData() << std::endl;
	}
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
		std::cout << "Enter 1 to set up Fat, 2 to create new file, 3 to show fat" << std::endl;
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
					createFat(nodes, splitStrings);
					fatIsSetUp = true;
				}
				else{
					std::cout << "Fat already set up" << std::endl;
				}
				break;
			/*case 2:
				std::cout << "Enter what to put into the file" << std::endl;
				std::cin >> userInput;
				std::string tempString = "This is my plain text that wil' be split up";
				//Fat::fat(userInput, numSplit);
				break;*/
			case 3:
				std::cout << "Showing File Allocation Table" << std::endl;
				printFat(nodes);
				break;
			default:
				std::cout << "You didn't enter a correct command" << std::endl;
				break;
		}
	}

	return 0;
}
