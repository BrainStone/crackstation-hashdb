#!/bin/bash

# Rebuild ./crackstation if necessary
if ! [ -x ./crackstation ]; then
  make release
fi

#hashTypes=( "md5" "sha1" "sha512" "NTLM" "LM" "MySQL4.1+" "md5(md5)" "whirlpool" )
hashTypes=( $(./crackstation -l) )
passed=true

mkdir -p test-index-files

for hash in "${hashTypes[@]}"; do
    echo "TESTING [$hash]..."
    
    if ! ./crackstation -cvqa "test/words.txt" "test-index-files/test-words-$hash.idx" "$hash"; then
      echo
      passed=false
    fi
done

echo ""

if $passed; then
  echo "Tests passed."
else
  echo "Tests failed!"
  echo "See errors above!"
fi
