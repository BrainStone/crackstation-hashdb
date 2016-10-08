#include "hashlib.h"

std::vector<std::string> HashLib::hashes( { "MD4", "MD5", "SHA-1", "SHA-224", "SHA-256", "SHA-384", "SHA-512", "MySQL4.1+", "RIPEMD-160", "Whirlpool", "LM", "NTLM" } );

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
	} else if ( (hashName == "ripemd160") || (hashName == "ripemd") ) {
		return new HashRIPEMD160();
	} else if ( hashName == "whirlpool" ) {
		return new HashWhirlpool();
	} else if ( hashName == "lm" ) {
		return new HashLM();
	} else if ( hashName == "ntlm" ) {
		return new HashNTLM();
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

HashLib::Hash HashLib::hash( const char * stringToHash ) {
	return hash( reinterpret_cast<const byte *>(stringToHash), strlen( stringToHash ) );
}

HashLib::Hash HashLib::hash( const std::string & stringToHash ) {
	return hash( reinterpret_cast<const byte *>(stringToHash.c_str()), stringToHash.length() );
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
	sstr << std::hex << std::uppercase << std::setfill( '0' );

	for ( const byte & byteToWrite : bytes ) {
		sstr << std::setw( 2 ) << (int)byteToWrite;
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
HashLib::Hash HashMD4::hash( const byte * stringToHash, size_t strSize ) {
	MD4( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashMD4::getLength() {
	return length;
}

// MD5
HashLib::Hash HashMD5::hash( const byte * stringToHash, size_t strSize ) {
	MD5( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashMD5::getLength() {
	return length;
}

// SHA-1
HashLib::Hash HashSHA1::hash( const byte * stringToHash, size_t strSize ) {
	SHA1( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA1::getLength() {
	return length;
}

// SHA-224
HashLib::Hash HashSHA224::hash( const byte * stringToHash, size_t strSize ) {
	SHA224( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA224::getLength() {
	return length;
}

// SHA-256
HashLib::Hash HashSHA256::hash( const byte * stringToHash, size_t strSize ) {
	SHA256( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA256::getLength() {
	return length;
}

// SHA-384
HashLib::Hash HashSHA384::hash( const byte * stringToHash, size_t strSize ) {
	SHA384( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA384::getLength() {
	return length;
}

// SHA-512
HashLib::Hash HashSHA512::hash( const byte * stringToHash, size_t strSize ) {
	SHA512( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashSHA512::getLength() {
	return length;
}

// MySQL4.1+
HashLib::Hash HashMySQL41::hash( const byte * stringToHash, size_t strSize ) {
	SHA1( stringToHash, strSize, hashStorage );
	SHA1( hashStorage, length, hashStorage2 );

	return HashLib::Hash( hashStorage2, length );
}

HashLib::size_type HashMySQL41::getLength() {
	return length;
}

// RIPEMD-160
HashLib::Hash HashRIPEMD160::hash( const byte * stringToHash, size_t strSize ) {
	RIPEMD160( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashRIPEMD160::getLength() {
	return length;
}

// Whirlpool
HashLib::Hash HashWhirlpool::hash( const byte * stringToHash, size_t strSize ) {
	WHIRLPOOL( stringToHash, strSize, hashStorage );

	return HashLib::Hash( hashStorage, length );
}

HashLib::size_type HashWhirlpool::getLength() {
	return length;
}

// LM
HashLib::Hash HashLM::hash( const byte * stringToHash, size_t strSize ) {
	std::array<byte, 14> paddedString( { 0 } );

	for ( size_t i = 0; i < std::min<size_t>( strSize, 14 ); i++ )
		paddedString[i] = toupper( stringToHash[i] );

	DESencrypt( paddedString.data(), hashStorage );
	DESencrypt( paddedString.data() + 7, hashStorage + 8 );

	return Hash( hashStorage, length );
}

HashLib::size_type HashLM::getLength() {
	return length;
}

void HashLM::DESencrypt( const byte * data, byte * storage ) {
	std::array<byte, 8> key( { 0 } );
	DES_key_schedule keysched;

	key[0] = (data[0] & 254);
	key[1] = (data[0] << 7) | (data[1] >> 1);
	key[2] = (data[1] << 6) | (data[2] >> 2);
	key[3] = (data[2] << 5) | (data[3] >> 3);
	key[4] = (data[3] << 4) | (data[4] >> 4);
	key[5] = (data[4] << 3) | (data[5] >> 5);
	key[6] = (data[5] << 2) | (data[6] >> 6);
	key[7] = (data[6] << 1);

	DES_set_key( reinterpret_cast<const_DES_cblock *>(key.data()), &keysched );

	DES_ecb_encrypt( reinterpret_cast<const_DES_cblock *>(message), reinterpret_cast<const_DES_cblock *>(storage), &keysched, DES_ENCRYPT );
}

// NTLM
HashLib::Hash HashNTLM::hash( const byte * stringToHash, size_t strSize ) {
	// Using smart pointer to enable RAII
	std::unique_ptr<byte[]> paddedString( new byte[strSize * 2]() );

	for ( size_t i = 0; i < strSize; i++ )
		paddedString[2 * i] = stringToHash[i];

	return HashMD4::hash( paddedString.get(), strSize * 2 );
}
