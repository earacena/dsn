#ifndef BLOCK_H
#define BLOCK_H

class Block{
	public:
		Block();
		Block(std::string userInput);	//constructor
		Block(std::string userInput, std::string fileName);

		//getters and setters
		std::string getData();
		void setData(std::string data);
		std::string getFileName();
		void setFileName(std::string fileName);
	private:
		std::string data;
		std::string fileName;
};

Block::Block(){ //just here, shouldnt be used
	this->fileName = "temp";
	this->data = "bleh";
}

Block::Block(std::string userInput) {
	this->fileName = userInput;
}

Block::Block(std::string fileName, std::string userInput) {
	this->fileName = fileName;
	this->data = userInput;
}

//getters and setters
std::string Block::getData() {
	return data;
}

void Block::setData(std::string data) {
	this->data = data;
}

std::string Block::getFileName() {
	return fileName;
}

void Block::setFileName(std::string fileName) {
	this->fileName = fileName;
}

#endif
