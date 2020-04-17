#include <iostream> //couts
#include <vector> //data storage
#include "fat.cpp"	//include FAT
#include "node.cpp"	//nodes

int main (int argc, char *argv[]){
	std::string userInput;
	int userChoice;
	int numFiles;
	bool fatIsSetUp = false;
	std::vector<Node> nodes;

	while (true) 
	{
		std::cout << "Enter s to set up Fat, c to create new file" << std::endl;
		std::cin >> userChoice;

		switch (userChoice) 
		{
			/*case 2:
				std::cout << "Enter what to put into the file" << std::endl;
				std::cin >> userInput;
				std::string tempString = "This is my plain text that wil' be split up";
				//Fat::fat(userInput, numSplit);
				break;*/
			case 1:	//set up fat once.
				if (fatIsSetUp == false) {
					/*std::cout << "Enter input to store" << std::endl;
					std::cin >> userInput;
					std::cout << "enter amount of files: " << std::endl; 
					std::cin >> numFiles;*/
					userInput = "Hello there, i am a very long string";
					numFiles = 3;
					//nodes = new vector<Node>();

					Node::createFat(nodes, userInput, numFiles);	//creates nodes and puts it into nodes vector
					fatIsSetUp = true;
				}
				break;
			default:
				std::cout << "You didn't enter a correct command" << std::endl;
				break;
		}
	}

	return 0;
}
