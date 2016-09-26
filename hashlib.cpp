#include "hashlib.h"

HashLib * HashLib::getHasher( const std::string & hashName ) {
	std::string lowerHash;
	lowerHash.resize( hashName.size() );
	std::transform( hashName.begin(), hashName.end(), lowerHash.begin(), ::tolower );

	if ( lowerHash == "md4" ) {
		return new HashMD4();
	} else if ( lowerHash == "md5" ) {
		return new HashMD5();
	} else if ( lowerHash == "sha1" ) {
		return new HashSHA1();
	} else if ( lowerHash == "sha224" ) {
		return new HashSHA224();
	} else if ( lowerHash == "sha256" ) {
		return new HashSHA256();
	} else if ( lowerHash == "sha384" ) {
		return new HashSHA384();
	} else if ( lowerHash == "sha512" ) {
		return new HashSHA512();
	} else if ( (lowerHash == "mysql4.1+") || (lowerHash == "mysql4.1") || (lowerHash == "mysql41") || (lowerHash == "mysql") ) {
		return new HashMySQL41();
	} else if ( lowerHash == "whirlpool" ) {
		return new HashWhirlpool();
	} else {
		throw std::invalid_argument( "The hash type \"" + hashName + "\" is unknown!" );
	}
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

// MD4
HashLib::Hash & HashMD4::hash( const std::string & stringToHash ) {
	MD4( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashMD4::getLength() {
	return length;
}

// MD5
HashLib::Hash & HashMD5::hash( const std::string & stringToHash ) {
	MD5( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashMD5::getLength() {
	return length;
}

// SHA-1
HashLib::Hash & HashSHA1::hash( const std::string & stringToHash ) {
	SHA1( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashSHA1::getLength() {
	return length;
}

// SHA-224
HashLib::Hash & HashSHA224::hash( const std::string & stringToHash ) {
	SHA224( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashSHA224::getLength() {
	return length;
}

// SHA-256
HashLib::Hash & HashSHA256::hash( const std::string & stringToHash ) {
	SHA256( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashSHA256::getLength() {
	return length;
}

// SHA-384
HashLib::Hash & HashSHA384::hash( const std::string & stringToHash ) {
	SHA384( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashSHA384::getLength() {
	return length;
}

// SHA-512
HashLib::Hash & HashSHA512::hash( const std::string & stringToHash ) {
	SHA512( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashSHA512::getLength() {
	return length;
}

// MySQL4.1+
HashLib::Hash & HashMySQL41::hash( const std::string & stringToHash ) {
	SHA1( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );
	SHA1( hashStorage, length, hashStorage2 );

	return *(new HashLib::Hash( hashStorage2, length ));
}

HashLib::size_type HashMySQL41::getLength() {
	return length;
}

// Whirlpool
HashLib::Hash & HashWhirlpool::hash( const std::string & stringToHash ) {
	WHIRLPOOL( reinterpret_cast<const HashLib::byte*>(stringToHash.c_str()), stringToHash.length(), hashStorage );

	return *(new HashLib::Hash( hashStorage, length ));
}

HashLib::size_type HashWhirlpool::getLength() {
	return length;
}
