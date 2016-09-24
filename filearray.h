#ifndef CRACKSTATION_FILEARRAY_H
#define CRACKSTATION_FILEARRAY_H

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class FileArray {
public:
	typedef size_t posType;

	struct IndexEntry;
	class Iterator;

	FileArray( const std::string &fileName );
	FileArray( const std::string &fileName, posType cacheSize );
	~FileArray();

	posType getFileSize() const;
	posType getSize() const;
	posType getCacheSize() const;

	void readEntry( IndexEntry &entry, posType index );
	void writeEntry( const IndexEntry &entry, posType index );

	Iterator & begin();
	Iterator & end();

private:
	FileArray() = delete;
	FileArray( const FileArray & ) = delete;

	std::fstream file;
	const posType fileSize;
	const posType size;
	const posType cacheSize;
	std::vector<IndexEntry> cache;

	void loadCacheFromFile();
	void writeCacheToFile();
};

struct FileArray::IndexEntry {
	static constexpr posType hashSize = 8;
	static constexpr posType offsetSize = 6;
	static constexpr posType indexSize = hashSize + offsetSize;

	unsigned char hash[hashSize]; // First 64 bits of the hash
	unsigned char offset[offsetSize]; // Position of word in dictionary (48-bit little endian integer)

	FileArray::IndexEntry& operator=( const IndexEntry &copyFrom );

	bool operator==( const IndexEntry &lhs ) const;
	bool operator!=( const IndexEntry &lhs ) const;
	bool operator< ( const IndexEntry &lhs ) const;
	bool operator> ( const IndexEntry &lhs ) const;
	bool operator<=( const IndexEntry &lhs ) const;
	bool operator>=( const IndexEntry &lhs ) const;
} __attribute__( (__packed__) );

class FileArray::Iterator : public std::iterator<std::random_access_iterator_tag, IndexEntry, long int> {
public:

private:
	posType position;
	value_type value;
};

#endif
