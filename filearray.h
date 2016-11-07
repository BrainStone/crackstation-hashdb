#ifndef CRACKSTATION_FILEARRAY_H
#define CRACKSTATION_FILEARRAY_H

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "hashlib.h"
#include "progressbar.h"
#include "util.h"

class FileArray {
public:
	typedef size_t posType;

	struct IndexEntry;

	static posType getFileSize( const std::string & fileName );
	static posType getSize( const std::string & fileName );

	FileArray( const std::string &fileName, bool autoLoad = true );
	FileArray( const std::string &fileName, posType cacheSize, bool autoLoad = true );
	FileArray( const std::string &fileName, ProgressBar* progressBar, bool autoLoad = true );
	FileArray( const std::string &fileName, posType cacheSize, ProgressBar* progressBar, bool autoLoad = true );
	~FileArray();

	posType getFileSize() const;
	posType getSize() const;
	posType getCacheSize() const;

	void readEntry( IndexEntry &entry, posType index );
	void writeEntry( const IndexEntry &entry, posType index );

	void loadCacheFromFile();
	void writeCacheToFile();

private:
	FileArray() = delete;
	FileArray( const FileArray & ) = delete;

	std::fstream file;
	const posType fileSize;
	const posType size;
	const posType cacheSize;
	posType readPos;
	posType writePos;
	std::vector<IndexEntry> cache;
	ProgressBar* progressBar;
	bool autoLoad;
};

struct FileArray::IndexEntry {
	static constexpr posType hashSize = 8;
	static constexpr posType offsetSize = 6;
	static constexpr posType indexSize = hashSize + offsetSize;

	unsigned char hash[hashSize]; // First 64 bits of the hash
	unsigned char offset[offsetSize]; // Position of word in dictionary (48-bit little endian integer)

	void setHash( const HashLib::Hash & hashSource );
	void setOffset( std::streampos pos );

	std::streampos getOffset() const;

	IndexEntry& operator=( const IndexEntry & copy );

	bool operator==( const IndexEntry &lhs ) const;
	bool operator!=( const IndexEntry &lhs ) const;
	bool operator< ( const IndexEntry &lhs ) const;
	bool operator> ( const IndexEntry &lhs ) const;
	bool operator<=( const IndexEntry &lhs ) const;
	bool operator>=( const IndexEntry &lhs ) const;

	friend std::ostream & operator<<( std::ostream & rhs, const IndexEntry & lhs );
	friend std::istream & operator >> ( std::istream & rhs, IndexEntry & lhs );
} __attribute__( (__packed__) );

#endif
