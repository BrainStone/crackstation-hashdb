.PHONY: all clean test testclean

all: crackstation

release: all

debug: all

crackstation: crackstation.cpp
	g++ -std=c++11 -Wall -O3 crackstation.cpp -lpthread -lcrypto -lssl -o crackstation

clean:
	rm -f crackstation

test:
	./test/test.sh

testclean:
	rm -f test-index-files/test-words*.idx
	rm -fd test-index-files
