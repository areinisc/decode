#include "BitStreams.h"
#include <string>
#include <fstream>
using namespace std;

#include <iostream>

const int FULLWORD = 8;  // 8 bits per char to file

void OutBitStream::_clear() {
  buffer = 0;
  bufsize = 0;
}

OutBitStream::OutBitStream() {
  _clear();
}

OutBitStream::~OutBitStream() {
  close();
}


bool OutBitStream::isOpen() const {
  return ((const_cast<OutBitStream*>(this))->file.is_open());
}


// low level manipulations to add the given value into the current
// buffer writing the leftmost resulting byte to the file, and
// returning the number of bits which were leftover.
int OutBitStream::_rawdump(int value, int numbits) {
  int numMissing = FULLWORD-bufsize;
  int shift = (numbits-numMissing);
  int prefix = value>>shift;
  buffer <<= numMissing;
  buffer += prefix;
  file.put(char(buffer));
  byteswritten++;
  _clear();
  return (numbits-numMissing);
}


bool OutBitStream::open(const std::string& filename) {
  if (isOpen())
    close();
  file.open(filename.c_str(), ios::binary|ios::out);
  _clear();
  byteswritten=0;
  return isOpen();
}

void OutBitStream::write(int value, int numbits) {
  if (isOpen() && numbits>0) {

      if (byteswritten>=LIMIT) {
	cerr << "WARNING. OutBitStream is being automatically closed" << endl
	     << "         as the file size is surpassing a safe limit." << endl;
	close();
	return;
      }

    int cleanvalue = (value & ((1<<numbits) - 1));

    if (bufsize+numbits < FULLWORD) {
      // no real output yet;  buffer it all
      bufsize += numbits;
      buffer <<= numbits;
      buffer += cleanvalue;
    } else {
      // fill one word then recurse
      int bitsLeft = _rawdump(cleanvalue,numbits);
      int suffix = cleanvalue & ((1<<bitsLeft) - 1);
      write(suffix, bitsLeft);  // recurse
    }
  }
}

void OutBitStream::close() {
  // must flush if leftover in buffer
  if (isOpen()) {
    if (bufsize!=0) {
      _rawdump(0,FULLWORD-bufsize);   // force flush by padding with trailing zeroes
    }
    file.close();
  }
}





void InBitStream::_clear() {
  buffer = 0;
  bufsize = 0;
}

InBitStream::InBitStream() {
  _clear();
}

InBitStream::~InBitStream() {
  close();
}


bool InBitStream::isOpen() const {
  return ((const_cast<InBitStream*>(this))->file.is_open());
}

bool InBitStream::eof() const {
  return (bufsize==0 && file.eof());
}

bool InBitStream::open(const std::string& filename) {
  if (isOpen())
    close();
  file.open(filename.c_str(), ios::binary|ios::in);
  _clear();
  _prefetch();
  return isOpen();
}

void InBitStream::_prefetch() {
  if (isOpen() && bufsize==0) {
    // need more from file
    buffer = file.get();
    if (file.eof())
      buffer = 0;
    else
      bufsize=FULLWORD;
  }
}
    

int InBitStream::read(int numbits) {
  if (isOpen()) {
    int result = 0;
    if (numbits==1) {
      // just need one bit
      if (bufsize>0) {  // prefetch may have reached eof
	result = (buffer>>(bufsize-1));
	buffer -= result<<(bufsize-1);
	bufsize--;
	if (bufsize==0)
	  _prefetch();
      }
    } else {
      int i;
      for (i=0; i<numbits; i++)
	result = (result<<1) + read(1);   // use recursion
    }
    return result;
  }
  return -1;
}


void InBitStream::close() {
  // must flush if leftover in buffer
  if (isOpen()) {
    file.close();
    file.clear();  // required to clear eof bit in case of reopen
  }
}


