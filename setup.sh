#!/bin/bash

# Usecase setup script

# Remove old files
echo "[Setup] Removing old files..."
rm -r node1;
rm -r node2;
rm -r node3;

# Compile main directory code (node)
echo "[Setup] Compiling network code (node)...";
make;

# Compile FAT code
cd FAT;
echo "[Setup] Compiling FAT code (fat)...";
g++ main.cpp -o a -g

echo "[Setup] Creating folders...";
cd ..;
mkdir node1;
mkdir node2;
mkdir node3;

echo "[Setup] Moving node executable...";
cp node ./node1;
cp node ./node2;
cp node ./node3;

mkdir node1/FAT;
mkdir node2/FAT;
mkdir node3/FAT;

echo "[Setup] Setting up folders...";
cp FAT/a node1/FAT;
cp FAT/a node2/FAT;
cp FAT/a node3/FAT;

echo "[Setup] Moving FAT executable..."
mv node1/FAT/a node1/FAT/fat;
mv node2/FAT/a node2/FAT/fat;
mv node3/FAT/a node2/FAT/fat;

touch node1/nodes.txt;
touch node2/nodes.txt;
touch node3/nodes.txt;
