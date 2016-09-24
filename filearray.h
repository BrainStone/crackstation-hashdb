#ifndef CRACKSTATION_FILEARRAY_H
#define CRACKSTATION_FILEARRAY_H

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class FileArray {
public:
	struct IndexEntry;
	class Iterator;

	FileArray( const std::string &fileName );
	FileArray( const std::string &fileName, size_t cacheSize );
	~FileArray();

	size_t getFileSize() const;
	size_t getSize() const;
	size_t getCacheSize() const;

	void readEntry( IndexEntry &entry, size_t index );
	void writeEntry( const IndexEntry &entry, size_t index );

	Iterator & begin();
	Iterator & end();

private:
	FileArray() = delete;
	FileArray( const FileArray & ) = delete;

	std::fstream file;
	const size_t fileSize;
	const size_t size;
	const size_t cacheSize;
	std::vector<IndexEntry> cache;

	void loadCacheFromFile();
	void writeCacheToFile();
};

struct FileArray::IndexEntry {
	static constexpr size_t hashSize = 8;
	static constexpr size_t offsetSize = 6;
	static constexpr size_t indexSize = hashSize + offsetSize;

	unsigned char hash[hashSize]; // First 64 bits of the hash
	unsigned char offset[offsetSize]; // Position of word in dictionary (48-bit little endian integer)

	FileArray::IndexEntry& operator=( const FileArray::IndexEntry &copyFrom );

	bool operator==( const FileArray::IndexEntry &lhs ) const;
	bool operator!=( const FileArray::IndexEntry &lhs ) const;
	bool operator< ( const FileArray::IndexEntry &lhs ) const;
	bool operator> ( const FileArray::IndexEntry &lhs ) const;
	bool operator<=( const FileArray::IndexEntry &lhs ) const;
	bool operator>=( const FileArray::IndexEntry &lhs ) const;
} __attribute__( (__packed__) );

class FileArray::Iterator : public std::iterator<std::random_access_iterator_tag, FileArray::IndexEntry, long int> {
	difference_type bla;
};

#endif
