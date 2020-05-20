#include "blockchain.hpp"

Blockchain::Blockchain(int d) {
    TransactionData genesisData;
    genesisData.fileName = "N/A";
    genesisData.content = "N/A";
    genesisData.receiverNode = -1;
    genesisData.nodeBlock = 0;
    genesisData.timestamp = time(&genesisData.timestamp);

    Blocks genesisBlock(0, genesisData, "1"); // GenesisBlock only holds its HASH, everything else is N/A
    Chain.push_back(genesisBlock);   // set FIRST block of Chain to be the Genesis Block
    difficulty = d; // set it to whatever you want the difficulty of PoW to be.
}

// used for importing Blockchain, only sets the difficulty
Blockchain::Blockchain(int d, int random) {
    difficulty = d;
}

std::vector<Blocks> Blockchain::getChain() {
    return Chain;
}

void Blockchain::addBlock(TransactionData data) {
    Blocks newBlock(Chain.size(), data, Chain[Chain.size()-1].getHash(), difficulty);
    //   Before adding newBlock to Chain, the block has to produce a PoW. Send it to other nodes for verification
    //       if verified, newBlock will be added to the Blockchain.
    // std::string proof_of_work = newBlock.mineBlock(difficulty);
    
    Chain.push_back(newBlock);
}

// overload: used when importing Blockchain
void Blockchain::addBlock(TransactionData data, std::string hash, std::string prevhash, int index, std::string proof) {
    Blocks newBlock(index, data, prevhash, hash, proof);
    Chain.push_back(newBlock);
}

void Blockchain::printChain() {
    std::vector<Blocks>::iterator it;
    
    if (Chain.size() == 0) {
        std::cout << "You have not imported a Blockchain\n";
    }
    else {
        for (size_t b = 0; b < Chain.size(); b++) {
            Blocks currentBlock = Chain[b];

            std::cout << "\nBlock====================================================================";
            std::cout << "\nIndex: " << currentBlock.getIndex();
            std::cout << "\nHash: " << currentBlock.getHash();
            std::cout << "\nPrevHash: " << currentBlock.getPrevHash();
            // std::cout << "\nSenderKey: " << currentBlock.data_.senderKey_;
            std::cout << "\nFilename: " << currentBlock.getData().fileName;
            std::cout << "\nContent: " << currentBlock.getData().content;
            std::cout << "\nReceiverNode: " << currentBlock.getData().receiverNode;
            std::cout << "\nNodeBlock: " << currentBlock.getData().nodeBlock;
            std::cout << "\nTimestamp: " << currentBlock.getData().timestamp;
            std::cout << "\nProof: " << currentBlock.getProof();
            // std::cout << "\nIs block valid? " << currentBlock.isHashValid();
            std::cout << "\n";
        }
    }
    
}

bool Blockchain::isChainValid() {
    std::vector<Blocks>::iterator it;
    
    for (it = Chain.begin(); it != Chain.end(); ++it)
    {
        Blocks currentBlock = *it;
        if (!currentBlock.isHashValid())
        {
            return false;
        }

        if (it != Chain.begin()) // check if it is NOT the genesis block.
        {   
            Blocks previousBlock = *(it - 1);
            // compare current block's prevHash to the previous block's hash.
            if (currentBlock.getPrevHash() != previousBlock.getHash())
            {
                return false;
            }
            if (!currentBlock.isProofValid(difficulty)) // if the PoW does not match, then not valid chain.
            {
                return false;
            }
        }
    }
    return true;
}

/* Functions for the receiving node, when it receives txt file with updated blockchain from neighbor nodes*/
void exportChain(Blockchain Chain){
	std::cout << "Exporting Blockchain as blockchain.txt\n";
	std::ofstream myfile;
	myfile.open("blockchain.txt");

	std::vector<Blocks>::iterator it;
	std::vector<Blocks> blockchain = Chain.getChain();

    for (size_t b = 0; b < blockchain.size(); b++) {
        Blocks currentBlock = blockchain[b];

        myfile << "\nBlock====================================================================";
        myfile << "\nIndex: " << currentBlock.getIndex();
        myfile << "\nHash: " << currentBlock.getHash();
        myfile << "\nPrevHash: " << currentBlock.getPrevHash();
        // std::cout << "\nSenderKey: " << currentBlock.data_.senderKey_;
        myfile << "\nFilename: " << currentBlock.getData().fileName;
        myfile << "\nContent: " << currentBlock.getData().content;
        myfile << "\nReceiverNode: " << currentBlock.getData().receiverNode;
        myfile << "\nNodeBlock: " << currentBlock.getData().nodeBlock;
        myfile << "\nTimestamp: " << currentBlock.getData().timestamp;
		myfile << "\nProof: " << currentBlock.getProof();
        // std::cout << "\nIs block valid? " << currentBlock.isHashValid();
        myfile << "\n";
    }
	myfile.close();

}
std::string store_after_space(std::string line) { // IMPORT BLOCKCHAIN HELPER
	bool canStore = false;
	std::string toStore = "";
	for (char ch : line) {
		if(canStore) {
			toStore += ch;
		}
		if (isblank(ch)) {
			canStore = true;
		}
	}
	return toStore;
}
void importChain(const std::string file) {
	std::ifstream myfile;
	std::string line;
    Blockchain chain(2,-1);

	myfile.open(file);
	if (myfile) {
		while (getline(myfile, line)) { //empty line
			int index, receiverNode;
			size_t nodeBlock;
			time_t timestamp;
			std::string hash = "", prevHash = "", fileName = "", content = "", proof = "";

			std::string toStore = "";
			getline(myfile, line); 	// block=======
			getline(myfile, line);	// Index: (int)
			// index = std::stoi(store_after_space(line),nullptr,16);
			// index = std::stoi(toStore);
			toStore = store_after_space(line);
			std::istringstream issindex(toStore);
			for (char c : toStore) {
				issindex >> index;
			}

			getline(myfile, line); // Hash: (string)
			hash = store_after_space(line);

			getline(myfile, line); // PreviousHash: (string)
			prevHash = store_after_space(line);

			getline(myfile, line); // fileName: (string)
			fileName = store_after_space(line);

			getline(myfile, line); // content: (string)
			content = store_after_space(line);

			getline(myfile, line); // receiverNode: (int)
			toStore = store_after_space(line);
			std::istringstream issnode(toStore);
			for (char c : toStore) {
				issnode >> receiverNode;
			}
			// toStore = store_after_space(line);
			// receiverNode = std::stoi(toStore);
			// receiverNode = std::stoi(store_after_space(line),nullptr,16);
			
			getline(myfile, line); // nodeBlock: (size_t)
			toStore = store_after_space(line);
			std::istringstream iss(toStore);
			for (char c : toStore) {
				iss >> nodeBlock;
			}

			getline(myfile, line); // timestamp: (time_t)
			toStore = store_after_space(line);
			std::istringstream isstime(toStore);
			for (char c : toStore) {
				isstime >> timestamp;
			}

			getline(myfile, line); // proof: (string)
			proof = store_after_space(line);
			
			TransactionData data;
			data.fileName = fileName;
			data.content = content;
			data.receiverNode = receiverNode;
			data.nodeBlock = nodeBlock;
			data.timestamp = timestamp;
			chain.addBlock(data, hash, prevHash, index, proof); // Data of each transaction
		}
		if (chain.isChainValid()) {
			std::cout << "Chain validated, storing at blockchain.txt\n";
			exportChain(chain);
		}
		else {
			std::cout << "Invalid imported Blockchain\n";
		}
		myfile.close();
	}
	else {
		std::cout << "error opening file\n";
		return;
	}
}
