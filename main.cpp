#include "main.h"

const option::Descriptor usage[] =
{
	{ UNKNOWN    , 0, "" , ""      , option::Arg::None    , "Usage:\n"
															"  Display help:\n"
															"    crackstation -h\n\n"
															"  Create dictionary:\n"
															"    crackstation -c [-v] [test]... [-r <Size>] [-q] <wordlist> <dictionary> <hashtype>\n\n"
															"  Find hash in dictionary:\n"
															"    crackstation [-q] <wordlist> <dictionary> <hashtype> <hashes>...\n\n"
															"  Verify dictionary:\n"
															"    crackstation -v [test]... [-q] [wordlist] <dictionary> [hashtype]\n\n"
															"  List available hashes:\n"
															"    crackstation -l" },
	{ UNKNOWN    , 0, "" , ""      , option::Arg::None    , "\n\n\nModes:" },
	{ HELP       , 0, "h", "help"  , option::Arg::None    , "  -h, --help,         \tPrint usage and exit." },
	{ CREATE     , 0, "c", "create", option::Arg::None    , "  -c, --create        \tCreates the dictionary from the wordlist." },
	{ VERIFY     , 0, "v", "verify", option::Arg::None    , "  -v, --verify        \tVerifies that the dictionary is sorted." },
	{ LIST       , 0, "l", "list"  , option::Arg::None    , "  -l, --list          \tLists all available hashes separted by a space character.\n" },

	{ UNKNOWN    , 0, "" , ""      , option::Arg::None    , "General options:" },
	{ QUIET      , 0, "q", "quiet" , option::Arg::None    , "  -q, --quiet         \tDisables most output. Usefull for automated scripts.\n" },

	{ UNKNOWN    , 0, "" , ""      , option::Arg::None    , "Create options:" },
	{ RAM        , 0, "r", "ram"   , Arg::ULong           , "  -r, --ram=SIZE      \tHow much RAM (SIZE MiB) to use for the cache when sorting the index file. (Only used when -c is set).\n" },

	{ UNKNOWN    , 0, "" , ""      , option::Arg::None    , "Verify options:" },
	{ TESTS_ALL  , 0, "a", "all"   , option::Arg::None    , "  -a, --all           \tEnables all tests. If \"wordlist\" and \"hashtype\" are not specified all tests requiring them will be silently skipped!\n"
															"\tEquivalent to: -s -m" },
	{ TESTS_FAST , 0, "f", "fast"  , option::Arg::None    , "  -f, --fast          \tEnables all fast tests. If no tests are specified these tests will be run. If \"wordlist\" and \"hashtype\" are not specified all tests requiring them will be silently skipped!\n"
															"\tEquivalent to: -s -m RANDOM_FULL\n" },
	{ TEST_SORTED, 0, "s", "sorted", option::Arg::None    , "  -s, --sorted        \tChecks whether the index file is sorted." },
	{ TEST_MATCH , 0, "m", "match" , isMatchMode          , "  -m, --match[=MODE]  \tTries to hash and then find all or some entries from the wordlist (depending on the mode). See below for match modes. Requires \"wordlist\" and \"hashtype\" to be specified!" },

	{ UNKNOWN    , 0, "" ,  ""     , option::Arg::None    , "\nMatch Modes:\n"
															"  ALL:            \tGo through the entire word list and do full and partial matching. (Default)\n"
															"  ALL_FULL:       \tGo through the entire word list and only do full matching.\n"
															"  ALL_PARTIAL:    \tGo through the entire word list and only do partial matching.\n"
															"  RANDOM:         \tPick random elements from the word list and do full and partial matching.\n"
															"  RANDOM_FULL:    \tPick random elements from the word list and only do full matching.\n"
															"  RANDOM_PARTIAL: \tPick random elements from the word list and only do partial matching.\n\n"
															"Examples:\n"
															"  crackstation -c words.txt words-sha512.idx sha512\n"
															"  crackstation words.txt words-md5.idx md5 827CCB0EEA8A706C4C34A16891F84E7B" },
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
		mode = MODE_USAGE;
	}

	const bool quiet = options[QUIET];
	const size_t ram = ((options[RAM]) ? std::stoul( options[RAM].arg ) : defaultRam) * MB;

	if ( !quiet )
		for ( option::Option* opt = options[UNKNOWN]; opt; opt = opt->next() )
			std::cout << "Unknown option: " << opt->name << "\n";

	try {
		if ( mode == MODE_USAGE ) {
			std::cerr << usage[0].help << std::endl;
		}

		if ( mode == MODE_HELP ) {
			option::printUsage( std::cerr, usage, ProgressBar::getConsoleWidth() );
		}

		if ( (mode == MODE_CREATE) || (mode == MODE_CREATE_VERIFY) ) {
			const std::string wordlist( parse.nonOption( 0 ) );
			const std::string idxFile( parse.nonOption( 1 ) );
			const std::string hashName( parse.nonOption( 2 ) );

			createIDX( wordlist, idxFile, hashName, quiet );
			sortIDX( idxFile, ram, quiet );
		}

		if ( (mode == MODE_VERIFY) || (mode == MODE_CREATE_VERIFY) ) {
			const bool onlyIdxFile( nonOptions == 1 );
			const std::string wordlist( onlyIdxFile ? "" : parse.nonOption( 0 ) );
			const std::string idxFile( parse.nonOption( onlyIdxFile ? 0 : 1 ) );
			const std::string hashName( onlyIdxFile ? "" : parse.nonOption( 2 ) );

			const bool testsAll( options[TESTS_ALL] );
			const bool testsFast( options[TESTS_FAST] || (!testsAll && !options[TEST_SORTED] && !options[TEST_MATCH]) );

			const bool testSorted( options[TEST_SORTED] || testsAll || testsFast );
			const bool testMatch( (options[TEST_MATCH] || testsAll || testsFast) && !onlyIdxFile );

			const matchMode mMode( getMatchMode( (options[TEST_MATCH].arg == NULL) ? (testsFast ? "RANDOM_FULL" : "ALL") : options[TEST_MATCH].arg ) );

			FileArray fileArray( idxFile );
			const size_t indexElements = fileArray.getSize();
			ProgressBar progressBar( {
				{"Test: Sorted", testSorted ? indexElements : 0},
				{"Test: Match", testMatch ? (((mMode & modeAll) ? indexElements : (size_t)sqrt( indexElements )) * (((mMode & modeFull) + (mMode & modePartial)) * 2)) : 0}
			}, true );
			std::unordered_map<std::string, bool> testResults;
			std::vector<std::string> insertionOrder;
			bool result;

			std::cout << "\nRunning Tests:" << std::endl;
			progressBar.start();

			// Run tests here!
			if ( testSorted ) {
				testResults.insert( std::make_pair( "Sorted", runTestSorted( fileArray, progressBar ) ) );
				insertionOrder.push_back( "Sorted" );
			}

			if ( testMatch ) {
				// Test if the wordlist matches the 
			}

			progressBar.finish(true);

			// Display results
			std::cout << "\nTest Results:\n======================================" << std::endl;

			for ( std::string test : insertionOrder ) {
				result = testResults[test];
				test += ':';
				test.resize( 20, ' ' );

				std::cout << "\tTest: " << test << (result ? "\33[32mOK" : "\33[1;31mFailed!") << "\33[0m" << std::endl;
			}
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

matchMode getMatchMode( std::string str ) {
	static const std::unordered_map<std::string, matchMode> strToMatchMode {
		{ "ALL", MATCH_ALL }, { "ALL_FULL", MATCH_ALL_FULL }, { "ALL_PARTIAL", MATCH_ALL_PARTIAL },
		{ "RANDOM", MATCH_RANDOM }, { "RANDOM_FULL", MATCH_RANDOM_FULL }, { "RANDOM_PARTIAL", MATCH_RANDOM_PARTIAL }
	};

	strToUpper( str );

	try {
		return strToMatchMode.at( str );
	} catch ( const std::out_of_range & e ) {
		throw std::invalid_argument( "Unknown Match Mode \"" + str + "\"!" );
	}
}

option::ArgStatus isMatchMode( const option::Option & option, bool msg ) {
	if ( (option.arg == NULL) || (option.name[option.namelen] != '\0') || (option.arg[0] == '-') )
		return option::ARG_IGNORE;

	try {
		getMatchMode( option.arg );

		return option::ARG_OK;
	} catch ( const std::invalid_argument & e ) {
		if ( msg )
			std::cerr << e.what() << std::endl;

		return option::ARG_ILLEGAL;
	}
}
