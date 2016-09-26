#include "hashlib.h"

HashLib * HashLib::getHasher( const std::string & hashName ) {
	std::string lowerHash;
	lowerHash.resize( hashName.size() );
	std::transform( hashName.begin(), hashName.end(), lowerHash.begin(), ::tolower );

	if ( hashName == "sha1" ) {
		return new HashSHA1();
	} else if ( hashName == "sha256" ) {
		return new HashSHA256();
	} else if ( hashName == "sha512" ) {
		return new HashSHA512();
	} else {
		throw std::invalid_argument( "The hash type \"" + hashName + "\" is unknown!" );
	}
}

HashLib::size_type HashLib::getLength() {
	return length;
}

HashLib::Hash::Hash() {}

HashLib::Hash::Hash( byte * bytes, size_type size ) : bytes( bytes, bytes + size ) {}

HashLib::Hash::Hash( const Hash & lhs ) : bytes( lhs.bytes ) {}

HashLib::byte HashLib::Hash::getNthByte( HashLib::size_type index ) const {
	if ( index < bytes.size() )
		return bytes[index];
	else
		return 0;
}

HashLib::size_type HashLib::Hash::getLength() const {
	return bytes.size();
}

HashLib::Hash & HashLib::Hash::operator=( const Hash & lhs ) {
	bytes = lhs.bytes;

	return *this;
}

bool HashLib::Hash::operator==( const Hash & lhs ) const {
	return bytes == lhs.bytes;
}

bool HashLib::Hash::operator!=( const Hash & lhs ) const {
	return !(*this == lhs);
}

HashLib::byte HashLib::Hash::operator[]( size_type index ) const {
	return getNthByte( index );
}

// Hashes

HashLib::Hash & HashSHA1::hash( const std::string & stringToHash ) {
	SHA1( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, getLength() ));
}

HashLib::Hash & HashSHA256::hash( const std::string & stringToHash ) {
	SHA256( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, getLength() ));
}

HashLib::Hash & HashSHA512::hash( const std::string & stringToHash ) {
	SHA512( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, getLength() ));
}
