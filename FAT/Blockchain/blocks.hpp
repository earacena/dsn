#ifndef BLOCKS_H
#define BLOCKS_H

#include <cstdint>
#include <iostream>
#include <ctime> // to keep track of the time a transaction occurred
// #include "../block.cpp" // a file's block = content of each block in blockchain

// class Block; // forward declaration

struct TransactionData {
    std::string fileName;
    int receiverNode;
    size_t nodeBlock;
    std::string content;
    time_t timestamp;
};

class Blocks {
    public:
        // Constructor
        Blocks(size_t index, TransactionData data, std::string prevHash); // for the GenesisBlock
        Blocks(size_t index, TransactionData data, std::string prevHash, int difficulty);
        Blocks(size_t index, TransactionData data, std::string prevHash, std::string hash, std::string proof);

        // Getters
        size_t getIndex();
        std::string getHash();
        std::string getPrevHash();
        TransactionData getData();
        std::string mineBlock(int difficulty); // mine the block by producing the PoW.
        std::string getProof();

        bool isHashValid();
        bool isProofValid(int difficulty);

    private:
        size_t _index;
        int _nonce;
        std::string _blockHash;
        TransactionData _data;
        std::string _prevHash; // this value should equal the _blockHash of the previous block in blockchain
        std::string generateHash(); // can only be performed by the Node creating the block of transaction
        std::string _proof; // proof-of-work of the block

};

#endif