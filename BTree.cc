/*
 * BTree.cc - implementation of methods of class BTree as declared in BTree.h.
 *
 * Several of the methods in this file must be implemented by students for
 * cs222 project 4
 *
 * If this file is compiled with the symbol PROFESSOR_VERSION defined,
 * it expects a file of that name to exist, and uses the code from
 * that file in place of selected dummy methods left to be written by students.
 *
 * Starting code Copyright (c) 2001, 2002, 2003 - Russell C. Bjork
 * Updates copyright (c) 2016, 2019 - Russ Tuck
 *
 * Student Author: Adam Princiotta
 */

#include <iostream>
using std::cout;

#include "BTree.h"
#include "BTreeFile.h"
#include "BTreeBlock.h"
#include <stack>
BTree::BTree(string name)
: _file(* new BTreeFile(name))
{ }

#ifndef PROFESSOR_VERSION
//Returns true and the value if the key is in the tree. Returns false if the
//tree is empty or false if it is an empty tree or if it's not in the tree
bool BTree::lookup(string key, string & value) const
{
    BTreeFile::BlockNumber blockNum = _file.getRoot();
    if(blockNum == 0){
      return false;
    }
    else{
      return lookupAux(key, value, blockNum);
    }
}


//Auxilary function for lookup. Uses recusion that sets the current block to the
//block given by the block number, then stores the position the key would be
//in the the block. If it is in the block, then store the value and return true
//Otherwise, if the block is not a leaf, move to the child recursively
bool BTree::lookupAux(string key, string & value, BTreeFile::BlockNumber blockNum) const {
  BTreeBlock currBlock;
  if(_file.getBlock(blockNum, currBlock)){
    int pos = currBlock.getPosition(key);
    if(currBlock.getKey(pos) == key){
      value = currBlock.getValue(pos);
      return true;
    }
    if(!currBlock.isLeaf()){
      return lookupAux(key, value, currBlock.getChild(pos));
    }
  }
  return false;
}



//Inserts keys and values into the necessary block. If the tree empty, a new
//root is made and the key and value are inserted into the new root block.
//If the key is found in a block, the key and value are inserted to replace it.
//If the key is not found in a block, it works its way down to the leaf node
//while each block is pushed to the parentStack. If that insert makes the block
//need to split, then the promoted key and value are stored through split call.
//If there is nothing in the parentStack, then it is just inserted into the
//the block above. Otherwise it promotes and moves up until no split is needed.
//Help from Ari Dospassos
void BTree::insert(string key, string value){
  if(_file.getRoot() == 0){
    BTreeBlock root;
    BTreeFile::BlockNumber allocate = _file.allocateBlock();
    _file.setRoot(allocate);
    root.insert(0, key, value, (unsigned) 0);
    root.setNumberOfKeys(1);
    root.setChild(0, (unsigned) 0);
    _file.putBlock(allocate, root);
    _file.setRoot(allocate);
    }

  else{
    int blockNum = _file.getRoot();
    std::stack<BTreeFile::BlockNumber> parentStack;
    BTreeBlock currBlock;

    if(_file.getBlock(blockNum, currBlock)){
      int pos = currBlock.getPosition(key);
      while(!currBlock.isLeaf() && currBlock.getKey(pos) != key){
       parentStack.push(blockNum);
       blockNum = currBlock.getChild(pos);
       _file.getBlock(blockNum, currBlock);
       pos = currBlock.getPosition(key);
       }

       if(currBlock.getKey(pos) == key){
         currBlock.setValue(pos, value);
         _file.putBlock(blockNum, currBlock);
         return;
       }
       currBlock.insert(pos, key, value, 0);
       _file.putBlock(blockNum, currBlock);
       while(currBlock.splitNeeded()){
         string promotedKey;
         string promotedValue;
         BTreeBlock rBlock;
         currBlock.split(promotedKey, promotedValue, rBlock);
         _file.putBlock(blockNum, currBlock);
         BTreeFile::BlockNumber rBlockNum =_file.allocateBlock();
         _file.putBlock(rBlockNum, rBlock);

         if(parentStack.empty()){
           BTreeFile::BlockNumber newBlockNum =_file.allocateBlock();
           BTreeBlock newBlock;
           _file.setRoot(newBlockNum);
           newBlock.insert(0, promotedKey, promotedValue, rBlockNum);
           newBlock.setChild(0, blockNum);
           _file.putBlock(newBlockNum, newBlock);
         }
         else{
           BTreeFile::BlockNumber parentBlockNum = parentStack.top();
           parentStack.pop();
           BTreeBlock parentBlock;
           _file.getBlock(parentBlockNum, parentBlock);
           int newPosition = parentBlock.getPosition(key);
           parentBlock.insert(newPosition, promotedKey, promotedValue, rBlockNum);

           if(!parentBlock.splitNeeded()){
             _file.putBlock(parentBlockNum, parentBlock);
           }
           else{
             blockNum = parentBlockNum;
             currBlock = parentBlock;
           }
         }
       }
     }
   }
}

//Returns the number of keys in the tree, uses recursive auxiliary function
//countSubtree
int BTree::count() const
{
  BTreeFile::BlockNumber blockNum = _file.getRoot();
  if(blockNum == 0){
    return 0;
  }
  else{
    return countSubtree(blockNum);
  }
}

//Auxilary function for count(). Stores the number of nodes and recursively
//calls the function until it reachesthe end of the children. Then returns
//the numbers of nodes + the number of keys in the current block.
//Help from Issac Bleecker
int BTree::countSubtree(BTreeFile::BlockNumber blockNum) const {
    BTreeBlock currBlock;
    _file.getBlock(blockNum, currBlock);
    int numOfNodes;
    if(currBlock.isLeaf()){
      return currBlock.getNumberOfKeys();
    }
    else{
      numOfNodes = 0;
      unsigned pos = 0;
      while(pos < (currBlock.getNumberOfKeys() + 1)){
        numOfNodes += countSubtree(currBlock.getChild(pos));
        pos++;
      }
    }
    return numOfNodes + currBlock.getNumberOfKeys();
}

//Finds how far into the tree the key is. Returns the depth of the tree. The
//root returns 0 if the tree is in the root, -1 if it is not in the tree. Uses
//an auxiliary function
int BTree::depth(string key) const
{
  BTreeFile::BlockNumber blockNum = _file.getRoot();
  int depth = 0;
  if(blockNum == 0){
    return -1;
  }
  else{
    return depthAux(key, blockNum, depth);
  }

}

//Auxilary function for depth. Recursively calls itself while the key is not in
//the current block and it is not a leaf. If it finds the key, it returns the
//depth, but returns -1 if not found and 0 if it's in the root.
int BTree::depthAux(string key, BTreeFile::BlockNumber blockNum, int depth) const{
  BTreeBlock currBlock;
  if(_file.getBlock(blockNum, currBlock)){
    int pos = currBlock.getPosition(key);
    if(currBlock.getKey(pos) == key){
      return depth;
    }
    if(!currBlock.isLeaf()){
      depth++;
      return depthAux(key, currBlock.getChild(pos), depth);
    }
  }
  if(depth != 0){
    return -1;
  }
  return 0;
}


bool BTree::remove(string key)
{
    return false; // Student code goes here - remove this line
}


// Students change nothing below here.

#else

#define QUOTE(Q) #Q
#define INCLUDE_NAME(X) QUOTE(X)
#include INCLUDE_NAME(PROFESSOR_VERSION)

#endif

void BTree::print() const
{
    cout << "BTree in file ";
    printInfo();
    cout << endl;

    BTreeFile::BlockNumber root = _file.getRoot();
    if (root == 0) {
        cout << "Empty tree" << endl;
    } else {
        _file.printBlock(root, true, 1);
    }
}


void BTree::print(BTreeFile::BlockNumber blockNumber) const
{
    _file.printBlock(blockNumber, false, 1);
}


void BTree::printInfo() const
{
    _file.printHeaderInfo();
}


void BTree::printStats() const
{
    _file.printStats();
}


BTree::~BTree()
{
    delete (& _file);
}
