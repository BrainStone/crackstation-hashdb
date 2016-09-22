#ifndef CRACKSTATION_FILEARRAY_H
#define CRACKSTATION_FILEARRAY_H

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

constexpr size_t hashSize = 8;
constexpr size_t offsetSize = 6;
constexpr size_t indexSize = hashSize + offsetSize;

struct IndexEntry {
	unsigned char hash[hashSize]; // First 64 bits of the hash
	unsigned char offset[offsetSize]; // Position of word in dictionary (48-bit little endian integer)

	IndexEntry& operator=( const IndexEntry &copyFrom );
	bool operator<( const IndexEntry &lhs ) const;
} __attribute__( (__packed__) );

class FileArray {
public:
	FileArray( const std::string &fileName );
	FileArray( const std::string &fileName, size_t cacheSize );
	~FileArray();

	size_t getFileSize() const;
	size_t getSize() const;
	size_t getCacheSize() const;

	void readEntry( IndexEntry &entry, size_t index );
	void writeEntry( const IndexEntry &entry, size_t index );

private:
	FileArray() = delete;
	FileArray( FileArray const & ) = delete;

	std::fstream file;
	const size_t fileSize;
	const size_t size;
	const size_t cacheSize;
	std::vector<IndexEntry> cache;

	void loadCacheFromFile();
	void writeCacheToFile();
};

#endif
