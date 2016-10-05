#include "hashlib.h"

std::vector<std::string> HashLib::hashes( { "MD4", "MD5", "SHA-1", "SHA-224", "SHA-256", "SHA-384", "SHA-512", "MySQL4.1+", "Whirlpool" } );

HashLib * HashLib::getHasher( std::string hashName ) {
	removeChars( hashName, "-+., " );
	strToLower( hashName );

	if ( hashName == "md4" ) {
		return new HashMD4();
	} else if ( hashName == "md5" ) {
		return new HashMD5();
	} else if ( (hashName == "sha1") || (hashName == "sha") ) {
		return new HashSHA1();
	} else if ( hashName == "sha224" ) {
		return new HashSHA224();
	} else if ( hashName == "sha256" ) {
		return new HashSHA256();
	} else if ( hashName == "sha384" ) {
		return new HashSHA384();
	} else if ( hashName == "sha512" ) {
		return new HashSHA512();
	} else if ( (hashName == "mysql41") || (hashName == "mysql") ) {
		return new HashMySQL41();
	} else if ( hashName == "whirlpool" ) {
		return new HashWhirlpool();
	} else {
		throw std::invalid_argument( "The hash type \"" + hashName + "\" is unknown!" );
	}
}

const std::vector<std::string>& HashLib::getHashes() {
	return hashes;
}

std::string HashLib::getHashesStr( const std::string & delim ) {
	return join( hashes, delim );
}

HashLib::Hash::Hash() {}

HashLib::Hash::Hash( byte * bytes, size_type size ) : bytes( bytes, bytes + size ) {}

HashLib::Hash::Hash( const Hash & lhs ) : bytes( lhs.bytes ) {}

byte HashLib::Hash::getNthByte( HashLib::size_type index ) const {
	if ( index < bytes.size() )
		return bytes[index];
	else
		return 0;
}

HashLib::size_type HashLib::Hash::getLength() const {
	return bytes.size();
}

std::string HashLib::Hash::toString() const {
	std::ostringstream sstr;

	for ( const byte & byteToWrite : bytes ) {
		sstr << std::fixed << std::setprecision( 2 ) << std::hex << (int)byteToWrite;
	}

	return sstr.str();
}

void HashLib::Hash::fromString( std::string str ) {
	strToLower( str );
	keepChars( str, "0123456789abcdef" );

	const size_t newSize = str.size() / 2;

	bytes.resize( newSize );

	for ( size_t i = 0; i < newSize; i++ ) {
		bytes[i] = (hexCharToByte( str[2 * i] ) << 4) | hexCharToByte( str[2 * i + 1] );
	}
}

bool HashLib::Hash::partialMatch( const Hash & lhs ) const {
	for ( size_t i = 0; i < bytes.size(); i++ ) {
		if ( bytes[i] != lhs.bytes[i] )
			return false;
	}

	return true;
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

byte HashLib::Hash::operator[]( size_type index ) const {
	return getNthByte( index );
}

std::ostream & operator<<( std::ostream & rhs, const HashLib::Hash & lhs ) {
	rhs << lhs.toString();

	return rhs;
}

// Hashing algorithms:
// MD4
HashLib::Hash HashMD4::hash( const std::string & stringToHash ) {
	MD4( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashMD4::getLength() {
	return length;
}

// MD5
HashLib::Hash HashMD5::hash( const std::string & stringToHash ) {
	MD5( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashMD5::getLength() {
	return length;
}

// SHA-1
HashLib::Hash HashSHA1::hash( const std::string & stringToHash ) {
	SHA1( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA1::getLength() {
	return length;
}

// SHA-224
HashLib::Hash HashSHA224::hash( const std::string & stringToHash ) {
	SHA224( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA224::getLength() {
	return length;
}

// SHA-256
HashLib::Hash HashSHA256::hash( const std::string & stringToHash ) {
	SHA256( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA256::getLength() {
	return length;
}

// SHA-384
HashLib::Hash HashSHA384::hash( const std::string & stringToHash ) {
	SHA384( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA384::getLength() {
	return length;
}

// SHA-512
HashLib::Hash HashSHA512::hash( const std::string & stringToHash ) {
	SHA512( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA512::getLength() {
	return length;
}

// MySQL4.1+
HashLib::Hash HashMySQL41::hash( const std::string & stringToHash ) {
	SHA1( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );
	SHA1( hashStorage, length, hashStorage2 );

	return HashLib::Hash( hashStorage2, length );
}

HashLib::size_type HashMySQL41::getLength() {
	return length;
}

// Whirlpool
HashLib::Hash HashWhirlpool::hash( const std::string & stringToHash ) {
	WHIRLPOOL( reinterpret_cast<const byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashWhirlpool::getLength() {
	return length;
}
