#include "filearray.h"

FileArray::posType FileArray::getFileSize( const std::string & fileName ) {
	return std::ifstream( fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate ).tellg();
}

FileArray::posType FileArray::getSize( const std::string & fileName ) {
	return getFileSize( fileName ) / FileArray::IndexEntry::indexSize;
}

FileArray::FileArray( const std::string & fileName, bool autoLoad ) :
	FileArray( fileName, 0, NULL, autoLoad ) {}

FileArray::FileArray( const std::string & fileName, FileArray::posType cacheSize, bool autoLoad ) :
	FileArray( fileName, cacheSize, NULL, autoLoad ) {}

FileArray::FileArray( const std::string & fileName, ProgressBar* progressBar, bool autoLoad ) :
	FileArray( fileName, 0, progressBar, autoLoad ) {}

FileArray::FileArray( const std::string & fileName, posType cacheSize, ProgressBar* progressBar, bool autoLoad ) :
	file( fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate ),
	fileSize( file.tellg() ),
	size( fileSize / FileArray::IndexEntry::indexSize ),
	cacheSize( std::min( cacheSize, size ) ),
	cache( this->cacheSize ),
	progressBar( progressBar ),
	autoLoad( autoLoad ) {
	if ( !file.good() ) {
		throw std::invalid_argument( "File \"" + fileName + "\" does not exist!" );
	} else if ( (FileArray::IndexEntry::indexSize * size) != fileSize ) {
		throw std::invalid_argument(
			"The file size needs to be divisible by " + std::to_string( FileArray::IndexEntry::indexSize ) + "!\n" +
			"File size of \"" + fileName + "\" is " + std::to_string( fileSize ) + "!" );
	}

	if ( autoLoad )
		loadCacheFromFile();
}

FileArray::~FileArray() {
	if ( autoLoad )
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
		file >> entry;
	}
}

void FileArray::writeEntry( const IndexEntry & entry, FileArray::posType index ) {
	if ( index < cacheSize ) {
		cache[index] = entry;
	} else if ( index < size ) {
		file.seekp( FileArray::IndexEntry::indexSize * index );
		file << entry;
	}
}

void FileArray::loadCacheFromFile() {
	size_t i = 0;

	file.seekg( 0 );

	for ( IndexEntry & entry : cache ) {
		file >> entry;

		if ( progressBar != NULL )
			progressBar->updateProgress( 0, ++i, cacheSize );
	}
}

void FileArray::writeCacheToFile() {
	const size_t lastSegment = (progressBar != NULL) ? (progressBar->getNumSegments() - 1) : 0;
	size_t i = 0;

	file.seekp( 0 );

	for ( const IndexEntry & entry : cache ) {
		file << entry;

		if ( progressBar != NULL )
			progressBar->updateProgress( lastSegment, ++i, cacheSize );
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

std::streampos FileArray::IndexEntry::getOffset() const {
	std::streampos pos = 0;

	for ( size_t i = 0; i < FileArray::IndexEntry::offsetSize; i++ ) {
		pos = (pos << 8) | offset[FileArray::IndexEntry::offsetSize - i - 1];
	}

	return pos;
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

std::ostream & operator<<( std::ostream & rhs, const FileArray::IndexEntry & lhs ) {
	rhs.write( reinterpret_cast<const char*>(&lhs), FileArray::IndexEntry::indexSize );

	return rhs;
}

std::istream & operator >> ( std::istream & rhs, FileArray::IndexEntry & lhs ) {
	rhs.read( reinterpret_cast<char*>(&lhs), FileArray::IndexEntry::indexSize );

	return rhs;
}
