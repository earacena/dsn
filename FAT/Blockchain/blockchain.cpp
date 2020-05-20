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