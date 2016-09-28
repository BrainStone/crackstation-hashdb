#include "util.h"

size_t getNumCores() {
	size_t out = std::thread::hardware_concurrency();

	if ( out == 0 )
		return 1;
	else
		return out;
}

unsigned short getBytePower( std::streampos size ) {
	unsigned short power;

	for ( power = 0; size >= 1000; power++ )
		size = size >> 10;

	return power;
}

std::string getBytePowerPostfix( unsigned short power ) {
	static const std::string postfixes[] = { "  B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
	static constexpr size_t numPostfixes = sizeof( postfixes ) / sizeof( std::string );

	if ( power > numPostfixes ) {
		return std::string( "2^" ) + std::to_string( power * 10 ) + postfixes[0];
	} else {
		return postfixes[power];
	}
}

std::string getFormatedSize( std::streampos size, int power ) {
	unsigned short formatPower = (power <= -1) ? getBytePower( size ) : (unsigned short)power;

	std::stringstream ss;

	if ( power == 0 ) {
		ss << std::setw( 3 ) << size << "    ";
	} else {
		ss << std::setw( 7 ) << std::fixed << std::setprecision( 3 ) << double( size ) / double( 1 << (10 * power) );
	}

	ss << ' ' << getBytePowerPostfix( formatPower );

	return ss.str();
}

option::ArgStatus Arg::Long( const option::Option& option, bool msg ) {
	try {
		std::stol( option.arg );

		return option::ARG_OK;
	} catch ( const std::exception & ) {
		if ( msg )
			std::cerr << "Option '" << option.name << "' requires a numeric argument\n" << std::endl;

		return option::ARG_ILLEGAL;
	}
}

option::ArgStatus Arg::ULong( const option::Option& option, bool msg ) {
	try {
		std::stoul( option.arg );

		return option::ARG_OK;
	} catch ( const std::exception & ) {
		if ( msg )
			std::cerr << "Option '" << option.name << "' requires a non-negative numeric argument\n" << std::endl;

		return option::ARG_ILLEGAL;
	}
}

std::string& removeChars( std::string& s, const std::string& chars ) {
	s.erase( std::remove_if( s.begin(), s.end(), [&chars]( const char& c ) {
		return chars.find( c ) != std::string::npos;
	} ), s.end() );

	return s;
}

std::string centerString( size_t width, const std::string& str ) {
	size_t len = str.length();

	if ( width < len ) {
		return str;
	}

	int diff = width - len;
	int pad1 = diff / 2;
	int pad2 = diff - pad1;

	return std::string( pad1, ' ' ) + str + std::string( pad2, ' ' );
}
