#include "murmurhash.h"
#include "BloomFilter.hpp"

using namespace std;
using namespace BF;
/**
 * numIndex: hash count
 * size: size of bf_vec
 * bf_vec: bitmap
 * */
BloomFilter::BloomFilter(){
	numElement = 1024;
	bitsPerElement = 10;

	numIndex = (int)floor(0.693*bitsPerElement+ 0.5); // refer to levelDB
	size = numElement * bitsPerElement;

    makeBloomFilter();
}

/**
 * @param numElement_ represent the number of elements in this bloom filter
 * @param bitsPerElement_
 * */
BloomFilter::BloomFilter( int numElement_, int bitsPerElement_ ){
	numElement = numElement_;

    // For small n, we can see a very high false positive rate.  Fix it
    // by enforcing a minimum bloom filter length.
	bitsPerElement = bitsPerElement_ < 1 ? 1 : bitsPerElement_;

	numIndex = (int)floor(0.693*bitsPerElement+ 0.5); // refer to levelDB
	size = numElement * bitsPerElement;

	makeBloomFilter();
}

/**
 * recover from SST
 * */
BloomFilter::BloomFilter( int numElement_, int bitsPerElement_, vector<bool> bf_vec ) {
    this->bf_vec = bf_vec;
    this->numElement = numElement_;
    this->bitsPerElement = bitsPerElement_;
    numIndex = (int)floor(0.693*bitsPerElement+ 0.5);
    size = numElement * bitsPerElement;
}

// Initialize bloom filter, set size and all bits to 0
void BloomFilter::makeBloomFilter(){
	bf_vec.resize(size, 0);
}

// Add an element
void BloomFilter::program( string key ){
	vector<int> index( numIndex, 0 );
	getIndex( key, &index );

	for(int i=0; i<numIndex; i++){
		bf_vec[index[i]] = 1;
	}
}

// Query an element
bool BloomFilter::query( string key ){
	vector<int> index( numIndex, 0 );
	getIndex( key, &index );

	for(int i=0; i<numIndex; i++){
		if( bf_vec[index[i]] == 0 )
			return false;
	}

	return true; // positive
}

//
void BloomFilter::getIndex( string key, vector<int>* index ){
    // return 32-bit integer
	unsigned int h = MurmurHash2( key.c_str(), key.size(), 0xbc9f1d34 );

    // Instead of actually running k hash functions, the original hash
    // value shift is used to get the value of the post-order hash function
    // double hashing
	const unsigned int delta = (h>>17) | (h<<15); // Rotate right 17 bits
    // set numIndex bits to 1
	for( int i=0 ; i<numIndex ; i++ ){
		index->at(i) = h % size; // if numIndex is larger than size use mod
		h += delta;
	}

	return;
}

int BloomFilter::getIndexNum(){
	return numIndex;
}

int BloomFilter::getSize(){
	return size;
}

const vector<bool> &BloomFilter::getBfVec() const {
    return bf_vec;
}
