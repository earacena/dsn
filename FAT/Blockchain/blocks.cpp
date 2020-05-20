#include "blocks.hpp"
#include "sha256.h"

Blocks::Blocks(size_t index, TransactionData data, std::string prevHash) {
    _index = index;
    _nonce = 0;
    _data.fileName = data.fileName;
    _data.receiverNode = data.receiverNode;
    _data.nodeBlock = data.nodeBlock;
    _data.content = data.content;
    _data.timestamp = time(&data.timestamp); // _time set to current time (UNIX timestamp)-seconds elapsed since 12am 01/01/1970
    _prevHash = prevHash;
    _blockHash = generateHash();
    _proof = "100";
}

Blocks::Blocks(size_t index, TransactionData data, std::string prevHash, int difficulty) {
    _index = index;
    _nonce = 0;
    _data.fileName = data.fileName;
    _data.receiverNode = data.receiverNode;
    _data.nodeBlock = data.nodeBlock;
    _data.content = data.content;
    _data.timestamp = time(&data.timestamp); // _time set to current time (UNIX timestamp)-seconds elapsed since 12am 01/01/1970
    _prevHash = prevHash;
    _blockHash = generateHash();
    _proof = mineBlock(difficulty);
}

Blocks::Blocks(size_t index, TransactionData data, std::string prevHash, std::string hash, std::string proof) {
    _index = index;
    _nonce = 0;
    _data.fileName = data.fileName;
    _data.receiverNode = data.receiverNode;
    _data.nodeBlock = data.nodeBlock;
    _data.content = data.content;
    _data.timestamp = data.timestamp; // _time set to current time (UNIX timestamp)-seconds elapsed since 12am 01/01/1970
    _prevHash = prevHash;
    _blockHash = hash;
    _proof = proof;
}

std::string Blocks::getHash() {
    return _blockHash;
}

std::string Blocks::getPrevHash() {
    return _prevHash;
}

TransactionData Blocks::getData() {
    return _data;
}

size_t Blocks::getIndex() {
    return _index;
}

std::string Blocks::getProof() {
    return _proof;
}

std::string Blocks::generateHash() {
    std::string toHash = std::to_string(_index) + _data.content + _data.fileName + std::to_string(_data.nodeBlock) + std::to_string(_data.receiverNode) + std::to_string(_data.timestamp);
    std::string hash = sha256(std::to_string(_nonce) + toHash);

    return hash;
}

std::string Blocks::mineBlock(int d) {
    std::string firstD_chars = "";
    for (int i = 0; i < d; i++) {
        firstD_chars += '0';
    }

    std::string toHash = std::to_string(_index) + _data.content + _data.fileName + std::to_string(_data.nodeBlock) + std::to_string(_data.receiverNode) + std::to_string(_data.timestamp);
    std::string proof = sha256(std::to_string(_nonce) + toHash);

    // Keep hashing with an incrementing nonce (number only used once)
    //      Until the first d characters of the hash == 0
    while(proof.substr(0,d) != firstD_chars) {
        _nonce++;
        // std::cout << _nonce << ": " << proof << std::endl;
        proof = sha256(std::to_string(_nonce) + toHash);
    }
    _nonce = 0;
    
    return proof;
}

bool Blocks::isHashValid() {
    return generateHash() == _blockHash;
}

bool Blocks::isProofValid(int d) {
    std::string firstD_chars = "";
    for (int i = 0; i < d; i++) {
        firstD_chars += '0';
    }

    return (_proof.substr(0, d) == firstD_chars);
}