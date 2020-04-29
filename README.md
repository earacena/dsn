# Decentralized Storage Network (untitled)

### Description

A decentralized storage network built on a blockchain and P2P network.

### Usage

#### Clone the repo


#### Build

While inside the main directory of the project, type:

```
make
```

#### Running
Perform these steps in order:

Open three terminals, and they will be refered to as "terminal 1", "terminal 2",
and "terminal 3" respectively.

In terminal 2,:

```
cd node2
./node
>> 3333
>> 0
>> n
```

In terminal 3:

```
cd node3
./node
>> 4444
>> 0
>> n
```

In terminal 1:

```
./FAT/fat
>> 1
>> 2 
>> 3
>> 9
CTRL+C
./node
>> 2222
>> 127.0.0.1
>> 3333
>> 127.0.0.1
>> 4444
>> y
```

This will demonstrate the features currently implemented. CTRL+C to exit any of the nodes.
