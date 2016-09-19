#include "main.h"

#include "optionparser.h"

using namespace std;

enum  optionIndex {
	UNKNOWN, HELP, CREATE, VERIFY, QUIET
};
const option::Descriptor usage[] =
{
	{ UNKNOWN, 0, "" , ""      , option::Arg::None, "USAGE:\n"
													"  Create dictionary:\n"
													"    crackstation -c [-v] [-q] <wordlist> <dictionary> <hashtype>\n"
													"  Find hash in dictionary:\n"
													"    crackstation [-q] <wordlist> <dictionary> <hashtype> [hashes...]\n"
													"  Verify dictionary:\n"
													"    crackstation -v [-q] <dictionary>\n\n"
													"Options:" },
	{ HELP   , 0, "h", "help"  , option::Arg::None, "  --help,   -h   \tPrint usage and exit." },
	{ CREATE , 0, "c", "create", option::Arg::None, "  --create, -c   \tCreates the dictionary from the wordlist." },
	{ VERIFY , 0, "v", "verify", option::Arg::None, "  --verify, -v   \tVerifies that the dictionary is sorted." },
	{ QUIET  , 0, "q", "quiet" , option::Arg::None, "  --quiet,  -q   \tDisables most output. Usefull for automated scripts." },
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
		(options[CREATE] && (parse.nonOptionsCount() != 3)) ||
		 (!options[CREATE] && options[VERIFY] && (parse.nonOptionsCount() != 1)) ||
		 (!options[CREATE] && !options[VERIFY] && (parse.nonOptionsCount() <= 3)) ) {
		option::printUsage( cerr, usage );

		return 0;
	}

	if ( !options[QUIET] )
		for ( option::Option* opt = options[UNKNOWN]; opt; opt = opt->next() )
			cout << "Unknown option: " << opt->name << "\n";

	if ( options[CREATE] )
		createIDX( parse.nonOption( 0 ), parse.nonOption( 1 ), parse.nonOption( 2 ), options[QUIET] );
}
