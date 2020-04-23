#ifndef BLOCK_H
#define BLOCK_H

class Block{
	public:
		Block();
		Block(std::string userInput);	//constructor

		//getters and setters
		std::string getData();		
		void setData(std::string data);	
	private:
		std::string data;
};	

Block::Block(){
	this->data = "bleh";
}

Block::Block(std::string userInput) {
	this->data = userInput;	
}

//getters and setters
std::string Block::getData() {
	return data;
}

void Block::setData(std::string data) {
	this->data = data;
}

#endif 
