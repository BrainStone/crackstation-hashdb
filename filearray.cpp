#include "filearray.h"

FileArray::FileArray( const std::string & fileName ) :
	FileArray( fileName, 0 ) {}

FileArray::FileArray( const std::string & fileName, FileArray::posType cacheSize ) :
	file( fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate ),
	fileSize( file.tellg() ),
	size( fileSize / FileArray::IndexEntry::indexSize ),
	cacheSize( std::min( cacheSize, size ) ),
	cache( this->cacheSize ) {
	if ( !file.good() ) {
		throw std::invalid_argument( "File \"" + fileName + "\" does not exist!" );
	} else if ( (FileArray::IndexEntry::indexSize * size) != fileSize ) {
		throw std::invalid_argument(
			"The file size needs to be divisible by " + std::to_string( FileArray::IndexEntry::indexSize ) + "!\n" +
			"File size of \"" + fileName + "\" is " + std::to_string( fileSize ) + "!" );
	}

	loadCacheFromFile();
}

FileArray::~FileArray() {
	writeCacheToFile();
}

FileArray::posType FileArray::getFileSize() const {
	return fileSize;
}

FileArray::posType FileArray::getSize() const {
	return size;
}

FileArray::posType FileArray::getCacheSize() const {
	return cacheSize;
}

void FileArray::readEntry( IndexEntry & entry, FileArray::posType index ) {
	if ( index < cacheSize ) {
		entry = cache[index];
	} else if ( index < size ) {
		file.seekg( FileArray::IndexEntry::indexSize * index );
		file.read( reinterpret_cast<char*>(&entry), FileArray::IndexEntry::indexSize );
	}
}

void FileArray::writeEntry( const IndexEntry & entry, FileArray::posType index ) {
	if ( index < cacheSize ) {
		cache[index] = entry;
	} else if ( index < size ) {
		file.seekp( FileArray::IndexEntry::indexSize * index );
		file.write( reinterpret_cast<const char*>(&entry), FileArray::IndexEntry::indexSize );
	}
}

void FileArray::loadCacheFromFile() {
	file.seekg( 0 );

	for ( IndexEntry & entry : cache ) {
		file.read( reinterpret_cast<char*>(&entry), FileArray::IndexEntry::indexSize );
	}
}

void FileArray::writeCacheToFile() {
	file.seekp( 0 );

	for ( const IndexEntry & entry : cache ) {
		file.write( reinterpret_cast<const char*>(&entry), FileArray::IndexEntry::indexSize );
	}
}

void FileArray::IndexEntry::setHash( const HashLib::Hash & hashSource ) {
	for ( size_t i = 0; i < FileArray::IndexEntry::hashSize; i++ ) {
		hash[i] = hashSource[i];
	}
}

void FileArray::IndexEntry::setOffset( std::streampos pos ) {
	for ( size_t i = 0; i < FileArray::IndexEntry::offsetSize; i++ ) {
		offset[i] = getNthByte( pos, i );
	}
}

FileArray::IndexEntry& FileArray::IndexEntry::operator=( const FileArray::IndexEntry & copy ) {
	FileArray::posType i;

	for ( i = 0; i < hashSize; i++ )
		hash[i] = copy.hash[i];

	for ( i = 0; i < offsetSize; i++ )
		offset[i] = copy.offset[i];

	return *this;
}

bool FileArray::IndexEntry::operator==( const FileArray::IndexEntry & lhs ) const {
	for ( FileArray::posType i = 0; i < hashSize; i++ ) {
		if ( hash[i] != lhs.hash[i] )
			return false;
	}

	return true;
}

bool FileArray::IndexEntry::operator!=( const FileArray::IndexEntry & lhs ) const {
	return !(*this == lhs);
}

bool FileArray::IndexEntry::operator<( const FileArray::IndexEntry &lhs ) const {
	for ( FileArray::posType i = 0; i < hashSize; i++ ) {
		if ( hash[i] < lhs.hash[i] )
			return true;
		else if ( hash[i] > lhs.hash[i] )
			return false;
	}

	return false;
}

bool FileArray::IndexEntry::operator>( const FileArray::IndexEntry & lhs ) const {
	return lhs < *this;
}

bool FileArray::IndexEntry::operator<=( const FileArray::IndexEntry & lhs ) const {
	return !(lhs < *this);
}

bool FileArray::IndexEntry::operator>=( const FileArray::IndexEntry & lhs ) const {
	return lhs <= *this;
}
