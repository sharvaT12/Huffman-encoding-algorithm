//
// the function of put, get, contains, and keys this si the template implementation
// of the huffman encoding algorithm
//

#include "hashmap.h"
#include <vector>
using namespace std;

// default constructor
//
// This constructor chooses number of buckets, initializes size of map to 0, and
// creates a bucket array with nBuckets number of pointers to linked lists.
//
hashmap::hashmap() {
    this->nBuckets=10;
    this->nElems=0;
    this->buckets=createBucketArray(nBuckets);
}

//
// Given a number of buckets, creates a hashtable (array of linked list heads).
// @param nBuckets the number of buckets you want in the hashtable.
// return an array of heads of linked lists of key_val_pairs
//
hashmap::bucketArray hashmap::createBucketArray(int nBuckets) {
    bucketArray newBuckets = new key_val_pair*[nBuckets];
    for (int i = 0; i < nBuckets; i++) {
        newBuckets[i] = nullptr;
    }
    return newBuckets;
}

//
// This destructor frees memory for all elements of buckets & also all elements
// of linked lists that those bucket pointers may point to.
//
hashmap::~hashmap() {
    for (int i=0; i < nBuckets; i++){
        key_val_pair *front =  buckets[i];
        while (front != nullptr) {
            key_val_pair *temp = front->next;
            delete front;
            front = temp;
        }
    }
    delete[] buckets;
    
}

//
// This method puts key/value pair in the map.  It checks to see if key is
// already in map while traversing the list to find the end of it.
//
void hashmap::put(int key, int value) {
  //
  // TODO: Implement this function.
  //
  // put a new key/ value pair into the hash table.
  // If the key already exists, update the value; 
  // otherwise, inser at the end of the linked list
  
  // get the value of the index
  int indexHash = hashFunction(key);
  // check if the inside is empty
  if (buckets[indexHash % nBuckets] == nullptr){
      // make a temp node 
      key_val_pair *node = new key_val_pair();
      node->next = nullptr;
      node->key = key;
      node->value = value;
      buckets[indexHash % nBuckets] = node;
      nElems++;
      return;
    }
   
    key_val_pair *first = buckets[indexHash % nBuckets];
    key_val_pair *last = first;
  // put a new key into hashtable
    while(first!=nullptr){
      if(first->key == key){
        first->value = value;
        return;
      }
      last = first;
      first = first->next;    
    }
      // pass the new table
      key_val_pair *node = new key_val_pair();
      node->next = nullptr;
      node->key = key;
      node->value = value;
      last->next = node;
      // update the number of elements
      nElems++;
}

//
// This method returns the value associated with key.
//
int hashmap::get(int key) const {
  //
  // TODO: Implement this function.
  //
  // throw an error if its not in the hashtable
  
  int indexHash = hashFunction(key);
  key_val_pair *first = buckets[indexHash % nBuckets];
  // the value % 10 isn't equal to null
  while(first!=nullptr){
    if(key == first->key){
      // return the value
      return first->value;
    }
    first = first->next;
  }
  // throw if it's error
  throw("Error because key is not in the hashtable");
}

//
// This function checks if the key is already in the map.
//
bool hashmap::containsKey(int key) {
  //
  // TODO: Implement this function.
  //
  int indexHash = hashFunction(key);
  key_val_pair *first = buckets[indexHash % nBuckets];
  // while the index%10 is not equal to null
  while(first!=nullptr){
    if(key == first->key){
      return true;
    }
    // if the first equal to next
    first = first->next;
  }
  return false;
}

//
// This method goes through all buckets and adds all keys to a vector.
//
vector<int> hashmap::keys() const {
  //
  // TODO: Implement this function.
  //
  // order should start from bucket 0 and go to bucket 9
  // key/value pairs should be added through normal front- back traversal of linked list
  // return vector of all the keys
  vector<int> keys;
  // while the value lower the nBuckets
  for(int i =0; i < nBuckets; i++){
    key_val_pair *first = buckets[i];
    // while the node is not equal to nullptr
    while(first != nullptr){
      // push the value
      keys.push_back(first->key);
      first = first->next;
    }
  }
  return keys;
}

//
// The hash function for hashmap implementation.
// For an extension, you might want to improve this function.
//
// @param input - an integer to be hashed
// return the hashed integer
//
int hashmap::hashFunction(int input) const {
    // use unsigned integers for calculation
    // we are also using so-called "magic numbers"
    // see https://stackoverflow.com/a/12996028/561677 for details
    unsigned int temp = ((input >> 16) ^ input) * 0x45d9f3b;
    temp = (temp >> 16) ^ temp;

    // convert back to positive signed int
    // (note: this ignores half the possible hashes!)
    int hash = (int) temp;
    if (hash < 0) {
        hash *= -1;
    }

    return hash;
}

//
// This function returns the number of elements in the hashmap.
//
int hashmap::size() {
    return nElems;
}

//
// Copy constructor
//
hashmap::hashmap(const hashmap &myMap) {
    // make a deep copy of the map
    nBuckets = myMap.nBuckets;

    buckets = createBucketArray(nBuckets);

    // walk through the old array and add all elements to this one
    vector<int> keys = myMap.keys();
    for (size_t i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        put(key,value);
    }

}

//
// Equals operator.
//
hashmap& hashmap::operator= (const hashmap &myMap) {
    // make a deep copy of the map

    // watch for self-assignment
    if (this == &myMap) {
        return *this;
    }

    // if data exists in the map, delete it
    for (int i=0; i < nBuckets; i++) {
        hashmap::key_val_pair* bucket = buckets[i];
        while (bucket != nullptr) {
            // walk the linked list and delete each node
            hashmap::key_val_pair* temp = bucket;
            bucket = bucket->next;
            delete temp;
        }
        buckets[i] = nullptr;
    }
    nElems = 0;
    // walk through the old array and add all elements to this one
    vector<int> keys = myMap.keys();
    for (size_t i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        put(key,value);
    }

    // return the existing object so we can chain this operator
    return *this;
}

//
// This function overloads the << operator, which allows for ease in printing
// to screen or inserting into a stream, in general.
//
ostream &operator<<(ostream &out, hashmap &myMap) {
    out << "{";
    vector<int> keys = myMap.keys();
    for (size_t i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        out << key << ":" << value;
        if (i < keys.size() - 1) { // no commas after the last one
            out << ", ";
        }
    }
    out << "}";
    return out;
}

//
// This function overloads the >> operator, which allows for ease at extraction
// from streams/files.
//
istream &operator>>(istream &in, hashmap &myMap) {
    // assume the format {1:2, 3:4}
    bool done = false;
    in.get(); // get the first char, {
    int nextChar = in.get(); // get the first real character
    while (!done) {
        string nextInput;
        while (nextChar != ',' and nextChar != '}') {
                nextInput += nextChar;
                nextChar = in.get();
        }
        if (nextChar == ',') {
            // read the space as well
            in.get(); // should be a space
            nextChar = in.get(); // get the next character
        } else {
            done = true; // we have reached }
        }
        // at this point, nextInput should be in the form 1:2
        // (we should have two integers separated by a colon)
        // BUT, we might have an empty map (special case)
        if (nextInput != "") {
            //vector<string> kvp;
            size_t pos = nextInput.find(":");
            myMap.put(stoi(nextInput.substr(0, pos)),
                      stoi(nextInput.substr(pos+1, nextInput.length() - 1)));
        }
    }
    return in;
}



