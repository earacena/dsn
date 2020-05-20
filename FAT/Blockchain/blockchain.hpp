#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <cstdint>
#include <vector> // Blockchain = vector of 'BLOCKS'
#include "blocks.hpp" // 'BLOCKS' class

// class Block;
class Blockchain {
    public:
        // Constructor
        Blockchain(int d); // allow user to enter difficulty of PoW algorithm
        Blockchain(int d, int random); // used for importing Blockchain, only sets the difficulty

        std::vector<Blocks> getChain();

        // Methods
        void addBlock(TransactionData data); 
        void addBlock(TransactionData data, std::string hash, std::string prevhash, int index, std::string proof);
        void printChain();
        bool isChainValid(); // Check whether the Blockchain is valid (by comparing current prevHash, to previous block's _blockHash)
            
    private:
        std::vector<Blocks> Chain;
        int difficulty; // used to set the difficulty of the Blockchain's PoW algorithm, the higher the more difficult.

};

#endif