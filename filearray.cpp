#include "filearray.h"

IndexEntry& IndexEntry::operator=( const IndexEntry &copyFrom ) {
	size_t i;

	for ( i = 0; i < hashSize; i++ )
		hash[i] = copyFrom.hash[i];

	for ( i = 0; i < offsetSize; i++ )
		offset[i] = copyFrom.offset[i];

	return *this;
}

bool IndexEntry::operator<( const IndexEntry &lhs ) const {
	for ( size_t i = 0; i < hashSize; i++ ) {
		if ( hash[i] < lhs.hash[i] )
			return true;
		else if ( hash[i] > lhs.hash[i] )
			return false;
	}

	return false;
}

FileArray::FileArray( const std::string & fileName ) :
	FileArray( fileName, 0 ) {}

FileArray::FileArray( const std::string & fileName, size_t cacheSize ) :
	file( fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate ),
	fileSize( file.tellg() ),
	size( fileSize / indexSize ),
	cacheSize( std::min( cacheSize, size ) ),
	cache( this->cacheSize ) {
	if ( (indexSize * size) != fileSize ) {
		throw std::invalid_argument(
			"The file size needs to be divisible by " + std::to_string( indexSize ) + "!\n" +
			"File size of \"" + fileName + "\" is " + std::to_string( fileSize ) + "!" );
	}

	loadCacheFromFile();
}

FileArray::~FileArray() {
	writeCacheToFile();
}

size_t FileArray::getFileSize() const {
	return fileSize;
}

size_t FileArray::getSize() const {
	return size;
}

size_t FileArray::getCacheSize() const {
	return cacheSize;
}

void FileArray::readEntry( IndexEntry & entry, size_t index ) {
	if ( index < cacheSize ) {
		entry = cache[index];
	} else {
		file.seekg( indexSize * index );
		file.read( reinterpret_cast<char*>(&entry), indexSize );
	}
}

void FileArray::writeEntry( const IndexEntry & entry, size_t index ) {
	if ( index < cacheSize ) {
		cache[index] = entry;
	} else {
		file.seekp( indexSize * index );
		file.write( reinterpret_cast<const char*>(&entry), indexSize );
	}
}

void FileArray::loadCacheFromFile() {
	file.seekg( 0 );

	for ( IndexEntry & entry : cache ) {
		file.read( reinterpret_cast<char*>(&entry), indexSize );
	}
}

void FileArray::writeCacheToFile() {
	file.seekp( 0 );

	for ( const IndexEntry & entry : cache ) {
		file.write( reinterpret_cast<const char*>(&entry), indexSize );
	}
}
