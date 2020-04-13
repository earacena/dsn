#include <iostream> //counts
#include <vector> //data storage
#include "fat.cpp"	//include FAT

int main (int argc, char *argv[]){
	std::string userInput;
	int numFiles = 3;

	while (true) 
	{
		std::cout << "Enter s to set up Fat, c to create new file" << std::endl;
		std::cin >> userInput;

		switch (userInput) 
		{
			case 'c':
				std::cout << "Enter what to put into the file" << std::endl;
				std::cin >> userInput;
				std::string tempString = "This is my plain text that wil' be split up";
				//Fat::fat(userInput, numSplit);
				break;
			case 's':
				std::cout << "Enter input" << std::endl;
				std::cin >> userInput;
				std::cout << "enter amount of files: " << std::endl; 
				std::cin >> numFiles;
				Fat::fat(userInput, numFiles);
				break;
			default:
				std::cout << "You didn't enter a correct command" << std::endl;
				break;
		}
	}

	return 0;
}