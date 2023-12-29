//
// STARTER CODE: util.h
//
// We are building a file compression algortihm based on binary tees and priority queues. The program
// will alloq us to compress and decompress the file with standard huffman algorithm
// for encoding and decoding. We re making a losless compression
// that will reduced the storage it require by a third, half, or even more, in some sitation.
//

#pragma once
#include <fstream>
#include <functional>  // std::greater
#include <iostream>
#include <map>
#include <queue>  // std::priority_queue
#include <string>
#include <unordered_map>
#include <vector>  // std::vector

#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"
#include "util.h"

struct HuffmanNode {
  int character;
  int count;
  HuffmanNode* zero;
  HuffmanNode* one;
};

class prioritize {
 public:
  bool operator()(const HuffmanNode* p1, const HuffmanNode* p2) const {
    // huffmanNode* will be different
    return p1->count > p2->count;
  }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree_recur(HuffmanNode* node) {
  // if the node is not empty it will empty inside
  if (node != nullptr) {
    freeTree_recur(node->zero);
    freeTree_recur(node->one);
    // delete the node
    delete node;
  }
  return;
}

void freeTree(HuffmanNode* node) {
  // TO DO:  Write this function here.
  // call the recursive function
  freeTree_recur(node);
  node = nullptr;
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
// MS 1
void buildFrequencyMap(string filename, bool isFile, hashmap& map) {
  // TO DO:  Write this function here.
  // if the the file can't be open
  if (!isFile) {
    // every character of inside the filename
    for (char a : filename) {
      int temp = int(a);
      // if inside has a value
      if (map.containsKey(temp)) {
        int mapPass = map.get(temp);
        // put in the value
        map.put(temp, mapPass + 1);
      } else {
        map.put(temp, 1);
      }
    }
  } else {
    // when the file can be open
    ifstream inFs(filename);
    char c;
    // if isFile is true, filename is an actual filename
    // otherwise, treat filename as a string variable
    while (inFs.get(c)) {
      int temp = int(c);
      if (map.containsKey(temp)) {
        int mapPass = map.get(temp);
        map.put(temp, mapPass + 1);
      } else {
        map.put(temp, 1);
      }
    }
  }
  // when it reach the end of the string
  //PSEUDO_EOF is 256
  // at the end of the string add it to the map
  map.put(PSEUDO_EOF, 1);
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmap& map) {
  // TO DO:  Write this function here.
  priority_queue<HuffmanNode*, vector<HuffmanNode*>, prioritize> pq;
  // binary tree node
  vector<int> key_node = map.keys();
  int size = key_node.size();
  for (int i = 0; i < size; i++) {
    HuffmanNode* curr = new HuffmanNode();
    // think zero as left
    curr->zero = nullptr;
    // think of one as right
    curr->one = nullptr;
    curr->character = key_node[i];
    curr->count = map.get(key_node[i]);
    // default constructor empty queue
    pq.push(curr);
  }
  // return the number of the element
  while (pq.size() > 1) {
    // get the front of the queue
    HuffmanNode* point_1 = pq.top();
    // remove the front element
    pq.pop();
    HuffmanNode* point_2 = pq.top();
    pq.pop();
    // store the value of huffmanNode
    HuffmanNode* cur = new HuffmanNode();
    cur->zero = point_1;
    cur->one = point_2;
    int first = point_1->count;
    int second = point_2->count;
    cur->count = first + second;
    cur->character = NOT_A_CHAR;
    pq.push(cur);
  }
  // check if the queue is empty
  return pq.top();
}

void buildEncodingMapRecur(mymap<int, string>& encodingMap, HuffmanNode* tree,
                           string pass) {
  if (tree == nullptr) {
    return;
  }
  // recursive call to node->zero, pass by value
  // recursiver call to node->zero, str+="0"
  buildEncodingMapRecur(encodingMap, tree->zero, pass + "0");
  // recursive call to node->one, str += "1"
  buildEncodingMapRecur(encodingMap, tree->one, pass + "1");
  // condition for a leaf node
  if (tree->character != NOT_A_CHAR) {
    // leaf node
    encodingMap.put(tree->character, pass);
  }
}

//
// *This function builds the encoding map from an encoding tree.
// default constructor, empty map
mymap<int, string> buildEncodingMap(HuffmanNode* tree) {
  mymap<int, string> encodingMap;
  string pass = "";
  // TO DO:  Write this function here.
  // loop recursively
  buildEncodingMapRecur(encodingMap, tree, pass);

  return encodingMap;  // TO DO: update this return
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, mymap<int, string>& encodingMap,
              ofbitstream& output, int& size, bool makeFile) {
  string temp;
  temp = "";
  char a;
  if (!makeFile) {
    while (input.get(a)) {
      temp += encodingMap.get(a);
    }
    temp += encodingMap.get(PSEUDO_EOF);
  } else if (makeFile) {
    // if the makeFile is true
    while (input.get(a)) {
      if (encodingMap.contains(a)) {
        temp += encodingMap.get(a);
      }
    } temp += encodingMap.get(PSEUDO_EOF);
    // in the string the 1's and 0's are chars but the arguments for the writeBit()
    // functions are ints, therefore you need to handle that case as well
    for (auto c : temp) {
      if (c == '1') {
        output.writeBit(1);
      } else if (c == '0') {
        output.writeBit(0);
      }
    }
  }
  // get the size
  size = temp.size();
  return temp;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream& input, HuffmanNode* encodingTree, ofstream& output) {
  HuffmanNode* cur = encodingTree;
  string temp = "";
  // for each bit in the file stream
  // traverse the tree
  while (!input.eof()) {
    int bit = input.readBit();
    if (bit == 0) {
      cur = cur->zero;
    } else if (bit == 1) {
      // if bit is 1 go right
      cur = cur->one;
    }
    if (cur->one == nullptr) {
      if (cur->zero == nullptr) {
        if (cur->character == PSEUDO_EOF) {
          break;
        }
      }
    } else if (cur->zero == nullptr) {
      // if its equal to the nullptr
      if (cur->one == nullptr) {
        if (cur->character == PSEUDO_EOF) {
          break;
        }
      }
    }
    // if you reach a leaf add char to output string
    // make sure we don't actually output the psudo_eof
    if (cur->character != NOT_A_CHAR) {
      temp += cur->character;
      output.put(cur->character);
      cur = encodingTree;
    }
  }

  return temp;  // TO DO: update this return
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
  // belgian_waffle.txt
  // TO DO:  Write this function here
  hashmap frequencyMap;
  bool compress_param = true;
  buildFrequencyMap(filename, compress_param, frequencyMap);
  HuffmanNode* compressingtree = buildEncodingTree(frequencyMap);
  // opening the file
  ifstream inFile;
  inFile.open(filename);
  mymap<int, string> frequencyMyMap = buildEncodingMap(compressingtree);
  string temp;
  // output the file into outside
  ofbitstream output(filename + ".huf");
  // output your frequency map from before to the ofbitstream
  output << frequencyMap;
  int pass = 0;
  temp = encode(inFile, frequencyMyMap, output, pass, compress_param);
  // clean it up
  freeTree(compressingtree);
  return temp;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
  // TO DO:  Write this function here.
  int first = filename.find(".huf");

  if (first >= 0) {
    filename = filename.substr(0, first);
  }
  // frequency of the map
  first = filename.find(".");
  string first_line = filename.substr(0, first);
  string second_line = filename.substr(first, filename.length() - 1);
  // prepare an output stream to a new file you will make
  ofstream osOutput(first_line + "_unc" + second_line);
  // string parsing the filename would be suggested
  ifbitstream input;
  // encoding the map
  hashmap frequencyMap;
  input.open(filename + ".huf");
  // build your frequency map from the ifbitstream
  input >> frequencyMap;
  // prepare an output bit stream to a new .huf we create
  buildFrequencyMap(first_line, true, frequencyMap);
  HuffmanNode* cur = buildEncodingTree(frequencyMap);

  string temp = decode(input, cur, osOutput);
  freeTree(cur);
  return temp;  // TO DO: update this return
}