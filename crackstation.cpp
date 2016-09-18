#include <iostream>
#include <string>
#include <pthread.h>
#include <openssl/sha.h>

using namespace std;

#define NUM_THREADS     5

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

string hexStr(unsigned char* data, int len)
{
  string s(len * 2, ' ');
  for (int i = 0; i < len; ++i) {
    s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  return s;
}

void *PrintHello(void* toHash)
{
   string* strToHash = (string*)toHash;
   
   cout << "Hello World!" << endl;
   cout << "String: " << *strToHash << endl;
   
   unsigned char hash256[SHA256_DIGEST_LENGTH];
   unsigned char hash512[SHA512_DIGEST_LENGTH];
   
   SHA256((const unsigned char*)strToHash->c_str(), strToHash->length(), hash256);
   
   cout << "SHA256 of \"" << *strToHash << "\" is " << hexStr(hash256, sizeof hash256) << endl; 
   
   SHA512((const unsigned char*)strToHash->c_str(), strToHash->length(), hash512);
   
   cout << "SHA512 of \"" << *strToHash << "\" is " << hexStr(hash512, sizeof hash512) << endl; 
   
   delete strToHash;
   
   pthread_exit(NULL);
}

int main ()
{
   pthread_t threads[NUM_THREADS];
   int rc;
   int i;
   string testString = "TestString";
   
   for( i=0; i < NUM_THREADS; i++ ){
      cout << "main() : creating thread, " << i << endl;
      rc = pthread_create(&threads[i], NULL, 
                          PrintHello, (void*) new string(testString + to_string(i)) );
      if (rc){
         cout << "Error: unable to create thread," << rc << endl;
         
         return -1;
      }
   }
   
   pthread_exit(NULL);
}
