#!/bin/bash

# Usecase setup script

make;
mkdir node1;
mkdir node2;
echo "This is " > 1;
echo "a test " > 2;
echo "message for" > 3;
echo "a dsn project." > 4;
mv 1 ./node1;
mv 2 ./node1;
mv 3 ./node2;
mv 4 ./node2;
cp node ./node1;
cp node ./node2;
