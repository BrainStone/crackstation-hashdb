[CrackStation.net](http://crackstation.net/)'s Lookup Tables
============================================================

Note
----
This is *not* the original crackstation-hashdb repository by @defuse, but a
modified copy of it. In this version I try to rewrite all parts in C++ as a
single binary.  
Also this version is WIP and *not* completly functional at the moment!

Introduction
------------

The main program has two several modes:

1. *Creating a index file*  
   In order to perform lookups you need to create an index file (usually .idx)
   from your wordlist. All words in the file get hashed and the sorted for a
   fast lookup.

2. *Performing a lookup*  
   Once a index file has been created you can perform lookups with this program.
   They are incredbly fast thanks to the sorted index.  
   *Note*: You still need to keep the index file!

3. *Verifying and testing an index file*  
   To make sure the index file has been created properly or the wordlist has not
   been updated there are several tests available to verify that the index file
   is valid.  
   *Note*: Not implemented yet!

4. *Printing a list of supported hashes*  
   The programm prints a simple space separted list of all available hashes.  
   *(Feel free to create pull requests implementing more hashes)*

This allows the user to only having to use one program.

Building and Testing
--------------------

To create the binary run `make`. Make sure you have `libssl-dev` installed. If
not simply install it by running `sudo apt-get install libssl-dev` (or similar
depending on your distributuion or OS). Also make sure to have at least
`gcc-4.8` installed.  
You can install this program by running `sudo make install`. This allows the
programm being used system wide and without the `./` in the front.

To run the tests, just run `make test`, and then clean up the files the tests
created with `make testclean`.

Indexing a Dictionary
---------------------

Suppose you have a password dictionary in the file `words.txt` and you would
like to index it for MD5 and SHA1 cracking.

Simply run these commands:

    $ ./crackstation -c -r 256 words.txt words-md5.idx md5
    $ ./crackstation -c -r 256 words.txt words-sha1.idx sha1

The `-r` parameter is the maximum amount of memory `crackstation` is allowed to
use in MiB for sorting the index. The more memory you let it use, the faster it
will go. Give it as much as your system will allow. 256 MiB is the default
value.  
Use the `-q` flag in order to disable the progressbar.

You now have everything you need to crack MD5 and SHA1 hashes quickly.

Cracking Hashes
---------------

Once you have generated and sorted the index, you can use the program in lookup
mode. Simply run it like this:

    $ ./crackstation words.txt words-md5.idx md5 098F6BCD4621D373CADE4E832627B4F6 202CB962AC59075B964B07152D234B70
    $ ./crackstation words.txt words-sha1.idx sha1 A94A8FE5CCB19BA61C4C0873D391E987982FBBD3 40BD001563085FC35165329EA1FF5C5ECBDBBEEF
    
You can supply as many hashes to cracked as you wish.

You can also use a file containing the hashes speparated by newlines or other
whitespace characters like this:

    $ ./crackstation words.txt words-md5.idx md5 $(cat hashes.txt)

Adding Words
------------

Once a wordlist has been indexed, you can not modify the wordlist file without
breaking the indexes. Appending to the wordlist is safe in that it will not
break the indexes, but the words you append won't be indexed, unless you
re-generate the index. There is currently no way to add words to an index
without re-generating the entire index.

General Information
-------------------

If you need help with the parameters simply run `./crackstation -h`. This will
generate a message explaining all parameters and flags similar to the following:

    Usage:
      Display help:
        crackstation -h
    
      Create dictionary:
        crackstation -c [-v] [test]... [-r <Size>] [-q] <wordlist> <dictionary> <hashtype>
    
      Find hash in dictionary:
        crackstation [-q] <wordlist> <dictionary> <hashtype> <hashes>...
    
      Verify dictionary:
        crackstation -v [test]... [-q] [wordlist] <dictionary> [hashtype]
    
      List available hashes:
        crackstation -l
    
    
    Modes:
      -h, --help,         Print usage and exit.
      -c, --create        Creates the dictionary from the wordlist.
      -v, --verify        Verifies that the dictionary is sorted.
      -l, --list          Lists all available hashes separted by a space character.
    
    General options:
      -q, --quiet         Disables most output. Usefull for automated scripts.
    
    Create options:
      -r, --ram=SIZE      How much RAM (SIZE MiB) to use for the cache when sorting the index file.
                          (Only used when -c is set).
    
    Verify options:
      -a, --all           Enables all tests. If "wordlist" and "hashtype" are not specified all tests
                          requiring them will be silently skipped!
                          Equivalent to: -s -m
      -f, --fast          Enables all fast tests. If no tests are specified these tests will be run.
                          If "wordlist" and "hashtype" are not specified all tests requiring them
                          will be silently skipped!
                          Equivalent to: -s -m RANDOM_FULL
    
      -s, --sorted        Checks whether the index file is sorted.
      -m, --match[=MODE]  Tries to hash and then find all or some entries from the wordlist
                          (depending on the mode). See below for match modes. Requires "wordlist" and
                          "hashtype" to be specified!
    
    Match Modes:
      ALL:                Go through the entire word list and do full and partial matching. (Default)
      ALL_FULL:           Go through the entire word list and only do full matching.
      ALL_PARTIAL:        Go through the entire word list and only do partial matching.
      RANDOM:             Pick random elements from the word list and do full and partial matching.
      RANDOM_FULL:        Pick random elements from the word list and only do full matching.
      RANDOM_PARTIAL:     Pick random elements from the word list and only do partial matching.
    
    Examples:
      crackstation -c words.txt words-sha512.idx sha512
      crackstation words.txt words-md5.idx md5 827CCB0EEA8A706C4C34A16891F84E7B

By default this program is very verbose displaying progressbars and similar
whenever it makes sense. To disable this behavior for example to use it with
automated scripts simply add the `-q` flag.

TODOs
-----

* Implement a faster sorting algorithm.
* Implement index file verification
* Add more hashes

After the completion of these tasks a pull request to the main repo will be
made.

Helping
-------

At the moment you can mainly help out by implementing more hashing algorithms.
Please only use libraries that are installed by default or OpenSSL.

Credits
-------

The password list used as an example is from
http://www.whatsmypass.com/the-top-500-worst-passwords-of-all-time
