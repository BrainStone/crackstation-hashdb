#ifndef CRACKSTATION_HASHLIB_H
#define CRACKSTATION_HASHLIB_H

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <openssl/sha.h>

class HashLib {
public:
	typedef unsigned char byte;
	typedef std::vector<byte>::size_type size_type;

	class Hash;

	static HashLib * getHasher( const std::string & hashName );

	virtual Hash & hash( const std::string & stringToHash ) = 0;
	virtual size_type getLength() = 0;

protected:
	static constexpr size_type length = 0;
};

class HashLib::Hash {
public:
	Hash();
	Hash( byte* bytes, size_type size );
	Hash( const Hash & lhs );

	byte getNthByte( size_type index ) const;
	size_type getLength() const;

	Hash & operator=( const Hash & lhs );
	bool operator==( const Hash &lhs ) const;
	bool operator!=( const Hash &lhs ) const;
	byte operator[]( size_type index ) const;
private:
	std::vector<byte> bytes;
};

class HashSHA1 : public HashLib {
public:
	virtual Hash & hash( const std::string & stringToHash );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashSHA256 : public HashLib {
public:
	virtual Hash & hash( const std::string & stringToHash );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA256_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

class HashSHA512 : public HashLib {
public:
	virtual Hash & hash( const std::string & stringToHash );
	virtual size_type getLength();

protected:
	static constexpr size_type length = SHA512_DIGEST_LENGTH;

private:
	byte hashStorage[length];
};

#endif
