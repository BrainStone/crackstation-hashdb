#include "main.h"

enum  optionIndex {
	UNKNOWN, HELP, CREATE, VERIFY, LIST, QUIET, RAM
};
const option::Descriptor usage[] =
{
	{ UNKNOWN, 0, "" , ""      , option::Arg::None, "USAGE:\n"
													"  Create dictionary:\n"
													"    crackstation -c [-v] [-q] <wordlist> <dictionary> <hashtype>\n\n"
													"  Find hash in dictionary:\n"
													"    crackstation [-q] <wordlist> <dictionary> <hashtype> <hashes>...\n\n"
													"  Verify dictionary:\n"
													"    crackstation -v [-q] <dictionary>\n\n\n"
													"Options:" },
	{ HELP   , 0, "h", "help"  , option::Arg::None, "  -h, --help,    \tPrint usage and exit." },
	{ CREATE , 0, "c", "create", option::Arg::None, "  -c, --create   \tCreates the dictionary from the wordlist." },
	{ VERIFY , 0, "v", "verify", option::Arg::None, "  -v, --verify   \tVerifies that the dictionary is sorted." },
	{ LIST   , 0, "l", "list"  , option::Arg::None, "  -l, --list     \tLists all available hashes separted by a space character." },
	{ QUIET  , 0, "q", "quiet" , option::Arg::None, "  -q, --quiet    \tDisables most output. Usefull for automated scripts.\n" },
	{ RAM    , 0, "r", "ram"   , Arg::ULong       , "  -r, --ram      \tHow much RAM (MiB) to use for the cache when sorting the index file. (Only used when -c is set)." },
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
		return 1;

	if ( options[HELP] ||
		(options[CREATE] && !options[LIST] && (parse.nonOptionsCount() != 3)) ||
		 (!options[CREATE] && options[VERIFY] && !options[LIST] && (parse.nonOptionsCount() != 1)) ||
		 (!options[CREATE] && !options[VERIFY] && !options[LIST] && (parse.nonOptionsCount() <= 3)) ||
		 (options[LIST] && (parse.nonOptionsCount() != 0)) ) {
		option::printUsage( std::cerr, usage, ProgressBar::getConsoleWidth() );

		return 0;
	}

	const bool quiet = options[QUIET];
	const size_t ram = ((options[RAM]) ? std::stoul( options[RAM].arg ) : defaultRam) * MB;

	if ( !quiet )
		for ( option::Option* opt = options[UNKNOWN]; opt; opt = opt->next() )
			std::cout << "Unknown option: " << opt->name << "\n";

	try {
		if ( options[CREATE] ) {
			createIDX( parse.nonOption( 0 ), parse.nonOption( 1 ), parse.nonOption( 2 ), quiet );
			//sortIDX( parse.nonOption( 1 ), ram, quiet );
		}

		if ( options[VERIFY] ) {
			// Just for testing....
			sortIDX( parse.nonOption( 0 + (bool)options[CREATE] ), ram, quiet );
		}

		if ( options[LIST] ) {
			std::cout << HashLib::getHashesStr() << std::endl;
		}
	} catch ( const std::invalid_argument & e ) {
		std::cerr << e.what() << std::endl;

		return 1;
	}
}
