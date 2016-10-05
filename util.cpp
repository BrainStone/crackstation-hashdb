#include "util.h"

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

std::string & strToLower( std::string & s ) {
	std::transform( s.begin(), s.end(), s.begin(), ::tolower );

	return s;
}

std::string& removeChars( std::string& s, const std::string& chars ) {
	s.erase( std::remove_if( s.begin(), s.end(), [&chars]( const char& c ) {
		return chars.find( c ) != std::string::npos;
	} ), s.end() );

	return s;
}

std::string& keepChars( std::string& s, const std::string& chars ) {
	s.erase( std::remove_if( s.begin(), s.end(), [&chars]( const char& c ) {
		return chars.find( c ) == std::string::npos;
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

byte hexCharToByte( char c ) {
	switch ( c ) {
	case '0': return 0x0;
	case '1': return 0x1;
	case '2': return 0x2;
	case '3': return 0x3;
	case '4': return 0x4;
	case '5': return 0x5;
	case '6': return 0x6;
	case '7': return 0x7;
	case '8': return 0x8;
	case '9': return 0x9;
	case 'a':
	case 'A': return 0xA;
	case 'b':
	case 'B': return 0xB;
	case 'c':
	case 'C': return 0xC;
	case 'd':
	case 'D': return 0xD;
	case 'e':
	case 'E': return 0xE;
	case 'f':
	case 'F': return 0xF;
	default: return -1;
	}
}
