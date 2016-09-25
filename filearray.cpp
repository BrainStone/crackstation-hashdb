#include "filearray.h"

FileArray::FileArray( const std::string & fileName ) :
	FileArray( fileName, 0 ) {}

FileArray::FileArray( const std::string & fileName, FileArray::posType cacheSize ) :
	file( fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate ),
	fileSize( file.tellg() ),
	size( fileSize / FileArray::IndexEntry::indexSize ),
	cacheSize( std::min( cacheSize, size ) ),
	cache( this->cacheSize ) {
	if ( (FileArray::IndexEntry::indexSize * size) != fileSize ) {
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
	} else if(index < size) {
		file.seekp( FileArray::IndexEntry::indexSize * index );
		file.write( reinterpret_cast<const char*>(&entry), FileArray::IndexEntry::indexSize );
	}
}

FileArray::Iterator FileArray::begin() {
	return FileArray::Iterator( *this, 0 );
}

FileArray::Iterator FileArray::end() {
	return FileArray::Iterator( *this, size );
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

FileArray::Iterator::Iterator() {}

FileArray::Iterator::Iterator( const FileArray::Iterator & copyFrom ) :
	position( copyFrom.position ),
	fileArray( copyFrom.fileArray ) {
	value = copyFrom.value;
}

FileArray::Iterator::~Iterator() {
	writeValue();
}

FileArray::Iterator & FileArray::Iterator::operator=( const FileArray::Iterator & copy ) {
	writeValue();
	position = copy.position;
	readValue();

	return *this;
}

FileArray::Iterator & FileArray::Iterator::operator+=( FileArray::Iterator::difference_type diff ) {
	writeValue();
	position += diff;
	readValue();

	return *this;
}

FileArray::Iterator & FileArray::Iterator::operator-=( FileArray::Iterator::difference_type diff ) {
	return (*this += -diff);
}

FileArray::Iterator::reference FileArray::Iterator::operator*() {
	return value;
}

FileArray::Iterator::pointer FileArray::Iterator::operator->() {
	return &value;
}

FileArray::Iterator::reference FileArray::Iterator::operator[]( difference_type pos ) const {
	return *(*this + pos);
}

FileArray::Iterator & FileArray::Iterator::operator++() {
	return *this += 1;
}

FileArray::Iterator & FileArray::Iterator::operator--() {
	return *this -= 1;
}

FileArray::Iterator FileArray::Iterator::operator++( int ) {
	FileArray::Iterator tmp( *this );

	++*this;

	return tmp;
}

FileArray::Iterator FileArray::Iterator::operator--( int ) {
	FileArray::Iterator tmp( *this );

	--*this;

	return tmp;
}

FileArray::Iterator::difference_type FileArray::Iterator::operator-( const FileArray::Iterator & rhs ) const {
	return position - rhs.position;
}

FileArray::Iterator FileArray::Iterator::operator+( FileArray::Iterator::difference_type rhs ) const {
	return Iterator( *fileArray, position + rhs );
}

FileArray::Iterator FileArray::Iterator::operator-( FileArray::Iterator::difference_type rhs ) const {
	return Iterator( *fileArray, position - rhs );
}

FileArray::Iterator operator+( FileArray::Iterator::difference_type lhs, const FileArray::Iterator & rhs ) {
	return rhs + lhs;
}

bool FileArray::Iterator::operator==( const FileArray::Iterator & lhs ) const {
	return position == lhs.position;
}

bool FileArray::Iterator::operator!=( const FileArray::Iterator & lhs ) const {
	return !(*this == lhs);
}

bool FileArray::Iterator::operator<( const FileArray::Iterator & lhs ) const {
	return position < lhs.position;
}

bool FileArray::Iterator::operator>( const FileArray::Iterator & lhs ) const {
	return lhs < *this;
}

bool FileArray::Iterator::operator<=( const FileArray::Iterator & lhs ) const {
	return !(lhs < *this);
}

bool FileArray::Iterator::operator>=( const FileArray::Iterator & lhs ) const {
	return lhs <= *this;
}

FileArray::Iterator::Iterator( FileArray & fileArray, posType position ) :
	position( position ),
	fileArray( &fileArray ) {
	readValue();
}

void FileArray::Iterator::readValue() {
	fileArray->readEntry( value, position );
}

void FileArray::Iterator::writeValue() {
	fileArray->writeEntry( value, position );
}
