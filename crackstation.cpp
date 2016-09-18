#include <iostream>
#include <string>
#include <pthread.h>
#include <openssl/sha.h>

using namespace std;

#define NUM_THREADS     5

template<typename T>
struct hex_t
{
    T x;
};

template<typename T>
hex_t<T> hex(T x)
{
    hex_t<T> h = {x};
    return h;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, hex_t<T> h)
{
    char buffer[2 * sizeof(T)];
    for (auto i = sizeof buffer; i--; )
    {
        buffer[i] = "0123456789ABCDEF"[h.x & 15];
        h.x >>= 4;
    }
    os.write(buffer, sizeof buffer);
    return os;
}

void *PrintHello(void* toHash)
{
   string* strToHash = (string*)toHash;
   
   cout << "Hello World!" << endl;
   cout << "String: " << *strTohash << endl;
   
   char hash256[SHA256_DIGEST_LENGTH];
   char hash512[SHA512_DIGEST_LENGTH];
   
   SHA256(strToHash->c_str(), strToHash->length(), hash256);
   
   cout << "SHA256 of \"" << *strTohash << "\" is " << hex(hash256) << endl; 
   
   SHA512(strToHash->c_str(), strToHash->length(), hash512);
   
   cout << "SHA512 of \"" << *strTohash << "\" is " << hex(hash512) << endl; 
   
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
