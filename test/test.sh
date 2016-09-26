#!/bin/bash

set -e

# Rebuild ./crackstation if necessary
if ! [ -x ./crackstation ]; then
  make release
fi

#hashTypes=( "md5" "sha1" "sha512" "NTLM" "LM" "MySQL4.1+" "md5(md5)" "whirlpool" )
hashTypes=( "md4" "md5" "sha1" "sha224" "sha256" "sha384" "sha512" "MySQL4.1+" "whirlpool" )

mkdir -p test-index-files

for hash in "${hashTypes[@]}"; do
    echo "TESTING [$hash]..."
    ./crackstation -c -v "test/words.txt" "test-index-files/test-words-$hash.idx" "$hash"
    php test/test.php "$hash"
done

echo ""
echo "Tests passed."
