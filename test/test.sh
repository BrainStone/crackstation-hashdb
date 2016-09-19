#!/bin/bash

set -e

# Rebuild ./crackstation
make clean
make release

hashTypes=( "md5" "sha1" "NTLM" "LM" "MySQL4.1+" "md5(md5)" "whirlpool" )

mkdir -p test-index-files

for hash in "${hashTypes[@]}"; do
    echo "TESTING [$hash]..."
    ./crackstation -c -v "test/words.txt" "test-index-files/test-words-$hash.idx" "$hash"
    php test/test.php "$hash"
done

echo ""
echo "Tests passed."
