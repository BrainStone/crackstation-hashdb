#ifndef CRACKSTATION_HASHLIB_H
#define CRACKSTATION_HASHLIB_H

#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <openssl/des.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/whrlpool.h>

#include "util.h"

class HashLib {
public:
	typedef std::vector<byte>::size_type size_type;

	class Hash;

	static HashLib * getHasher( std::string hashName );
	static const std::vector<std::string> & getHashes();
	static std::string getHashesStr( const std::string & delim = " " );

	virtual Hash hash( const char * stringToHash );
	virtual Hash hash( const std::string & stringToHash );
	virtual Hash hash( const byte * stringToHash, size_t strSize ) = 0;
	virtual size_type getLength() = 0;

protected:
	static constexpr size_type length = 0;

private:
	static std::vector<std::string> hashes;
};

class HashLib::Hash {
public:
	Hash();
	Hash( byte* bytes, size_type size );
	Hash( const Hash & lhs );

	byte getNthByte( size_type index ) const;
	size_type getLength() const;

	std::string toString() const;
	void fromString( std::string str );
	bool partialMatch( const Hash &lhs ) const;

	Hash & operator=( const Hash & lhs );
	bool operator==( const Hash &lhs ) const;
	bool operator!=( const Hash &lhs ) const;
	byte operator[]( size_type index ) const;

	friend std::ostream & operator<<( std::ostream & rhs, const Hash & lhs );
private:
	std::vector<byte> bytes;
};

// Hashing algorithms:
class HashMD4 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = MD4_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashMD5 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = MD5_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashSHA1 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashSHA224 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA224_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashSHA256 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA256_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashSHA384 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA384_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashSHA512 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA512_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashMySQL41 : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA_DIGEST_LENGTH;

private:
	byte hashStorage[length];
	byte hashStorage2[length];
};

class HashWhirlpool : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = WHIRLPOOL_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashLM : public HashLib {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
	virtual size_type getLength();

protected:
	static constexpr size_type length = 16;

private:
	static constexpr char* message = const_cast<char *>("\x4b\x47\x53\x21\x40\x23\x24\x25");

	byte hashStorage[length];

	void DESencrypt( const byte * data, byte * storage );
};

class HashNTLM : public HashMD4 {
public:
	using HashLib::hash;
	virtual Hash hash( const byte * stringToHash, size_t strSize );
};

#endif
