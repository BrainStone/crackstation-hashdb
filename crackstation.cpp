#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <openssl/sha.h>

using namespace std;

#define NUM_THREADS     5
#define HASH_SIZE       8
#define OFFSET_SIZE     6
#define WRITE_SIZE      HASH_SIZE + OFFSET_SIZE

typedef lock_guard<mutex> scoped_lock;

/**
 * Gets the nth byte from a var. The 0th byte is the right most byte numerically speaking.
 * Or it gets the nth byte in little-endian.
 */
template<class T>
constexpr unsigned char getNthByte(T var, size_t pos) {
  return (unsigned char) var >> (pos * 8) & 0xFF;
}

void computeHashes(atomic<bool>& threadReady, mutex& fileInMutex, mutex& fileOutMutex, ifstream& fileIn, ofstream& fileOut) {
  int i;
  
  string line;
  streampos pos;
  unsigned char hash512[SHA512_DIGEST_LENGTH];
  unsigned char write_buffer[WRITE_SIZE];
  
  while (!fileIn.eof()) {
    {
      scoped_lock(fileInMutex);
      
      pos = fileIn.tellg();
      getline(fileIn, line);
    }
    
    SHA512((const unsigned char*)line.c_str(), line.length(), hash512);
    
    for (i = 0; i < HASH_SIZE; i++) {
      write_buffer[i] = hash512[i];
    }
    
    for (i = 0; i < OFFSET_SIZE; i++) {
      write_buffer[i + HASH_SIZE] = getNthByte(pos, i);
    }
    
    {
      scoped_lock(fileOutMutex);
      
      fileOut.write(write_buffer, WRITE_SIZE);
    }
  }
  
  threadReady = true; 
}

int main () {
  int i;
  
  thread threads[NUM_THREADS];
  atomic<bool> threadReady[NUM_THREADS];
  mutex fileInMutex();
  mutex fileOutMutex();
  ifstream fileIn("test/words.txt", ios::in | ios::ate);
  ofstream fileOut("test/words-sha512.idx", ios::out | ios::trunc);
  
  const streampos fileSize = fileIn.tellg();
  fileIn.seek(0);
  
  for (i = 0; i < NUM_THREADS; i++) {
    threadReady[i] = false;
    
    threads[i] = thread(threadReady[i], computeHashes, fileInMutex, fileOutMutex, fileIn, fileOut);
  }
  
  while (true) {
    this_thread::sleep_for(chrono::milliseconds(100));
    
    {
      scoped_lock(fileInMutex);
      
      cout << fileIn.tellg() << '/' << size_t << endl;
    }
    
    for (i = 0; i < NUM_THREADS; i++)
      if (!threadReady[i])
        continue;
    
    break;
  }
  
  fileIn.close();
  fileOut.close();
}
