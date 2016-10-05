#include "main.h"

enum  optionIndex {
	UNKNOWN, HELP, CREATE, VERIFY, LIST, QUIET, RAM
};
enum  programMode {
	MODE_HELP, MODE_CREATE, MODE_CREATE_VERIFY, MODE_VERIFY, MODE_LIST, MODE_SEARCH
};
const option::Descriptor usage[] =
{
	{ UNKNOWN, 0, "" , ""      , option::Arg::None, "Usage:\n"
													"  Create dictionary:\n"
													"    crackstation -c [-v] [-r <Size>] [-q] <wordlist> <dictionary> <hashtype>\n\n"
													"  Find hash in dictionary:\n"
													"    crackstation [-q] <wordlist> <dictionary> <hashtype> <hashes>...\n\n"
													"  Verify dictionary:\n"
													"    crackstation -v [-q] [wordlist] <dictionary> [hashtype]\n\n"
													"  List available hashes:\n"
													"    crackstation -l\n\n\n"
													"Options:" },
	{ HELP   , 0, "h", "help"  , option::Arg::None, "  -h, --help,     \tPrint usage and exit." },
	{ CREATE , 0, "c", "create", option::Arg::None, "  -c, --create    \tCreates the dictionary from the wordlist." },
	{ VERIFY , 0, "v", "verify", option::Arg::None, "  -v, --verify    \tVerifies that the dictionary is sorted." },
	{ LIST   , 0, "l", "list"  , option::Arg::None, "  -l, --list      \tLists all available hashes separted by a space character." },
	{ QUIET  , 0, "q", "quiet" , option::Arg::None, "  -q, --quiet     \tDisables most output. Usefull for automated scripts." },
	{ RAM    , 0, "r", "ram"   , Arg::ULong       , "  -r, --ram=SIZE  \tHow much RAM (SIZE MiB) to use for the cache when sorting the index file. (Only used when -c is set)." },
	{ UNKNOWN, 0, "" ,  ""     , option::Arg::None, "\nExamples:\n"
													"  crackstation -c words.txt words-sha512.idx sha512\n"
													"  crackstation words.txt words-md5.idx md5 827ccb0eea8a706c4c34a16891f84e7b\n" },
	{ 0,0,0,0,0,0 }
};

int main( int argc, char* argv[] ) {
	argc -= (argc > 0); argv += (argc > 0); // skip program name argv[0] if present
	option::Stats  stats( usage, argc, argv );
	option::Option options[stats.options_max], buffer[stats.buffer_max];
	option::Parser parse( usage, argc, argv, options, buffer );

	if ( parse.error() )
		return 2;

	programMode mode;
	const size_t nonOptions = parse.nonOptionsCount();

	if ( options[HELP] )
		mode = MODE_HELP;
	else if ( options[CREATE] )
		if ( options[VERIFY] )
			mode = MODE_CREATE_VERIFY;
		else
			mode = MODE_CREATE;
	else if ( options[VERIFY] )
		mode = MODE_VERIFY;
	else if ( options[LIST] )
		mode = MODE_LIST;
	else
		mode = MODE_SEARCH;

	if ( (((mode == MODE_CREATE) || (mode == MODE_CREATE_VERIFY)) && (nonOptions != 3)) ||
		((mode == MODE_VERIFY) && ((nonOptions != 1) && (nonOptions != 3))) ||
		 ((mode == MODE_LIST) && (nonOptions != 0)) ||
		 ((mode == MODE_SEARCH) && (nonOptions <= 3)) ) {
		mode = MODE_HELP;
	}

	const bool quiet = options[QUIET];
	const size_t ram = ((options[RAM]) ? std::stoul( options[RAM].arg ) : defaultRam) * MB;

	if ( !quiet )
		for ( option::Option* opt = options[UNKNOWN]; opt; opt = opt->next() )
			std::cout << "Unknown option: " << opt->name << "\n";

	try {
		if ( mode == MODE_HELP ) {
			option::printUsage( std::cerr, usage, ProgressBar::getConsoleWidth() );
		}

		if ( (mode == MODE_CREATE) || (mode == MODE_CREATE_VERIFY) ) {
			const std::string wordlist( parse.nonOption( 0 ) );
			const std::string idxFile( parse.nonOption( 1 ) );
			const std::string hashName( parse.nonOption( 2 ) );

			createIDX( wordlist, idxFile, hashName, quiet );
			//sortIDX(idxFile, ram, quiet );
		}

		if ( (mode == MODE_VERIFY) || (mode == MODE_CREATE_VERIFY) ) {
			const std::string idxFile( parse.nonOption( (nonOptions == 1) ? 0 : 1 ) );

			// Just for testing....
			sortIDX( idxFile, ram, quiet );
		}

		if ( mode == MODE_LIST ) {
			std::cout << HashLib::getHashesStr() << std::endl;
		}

		if ( mode == MODE_SEARCH ) {
			const std::string wordlist( parse.nonOption( 0 ) );
			const std::string idxFile( parse.nonOption( 1 ) );
			const std::string hashName( parse.nonOption( 2 ) );
			std::string hash;
			std::vector<Match> matches;

			for ( size_t i = 3; i < nonOptions; i++ ) {
				hash = parse.nonOption( i );
				matches = Match::getMatches( wordlist, idxFile, hashName, hash );

				if ( !quiet )
					std::cout << "Matches for hash " << hash << ":\n";

				if ( !matches.empty() )
					std::cout << join( matches, "\n" ) << (quiet ? "\n" : "\n\n");
				else if ( !quiet )
					std::cout << "\33[1;30m\tnone\33[0m\n\n";

				std::cout.flush();
			}
		}
	} catch ( const std::invalid_argument & e ) {
		std::cerr << e.what() << std::endl;

		return 1;
	}

	return 0;
}
