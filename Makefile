.PHONY: all clean test testclean

all: crackstation

crackstation: crackstation.cpp
	g++ -Wall -O3 crackstation.cpp -lpthread -o crackstation

clean:
	rm -f crackstation

test:
	./test/test.sh

testclean:
	rm -f test-index-files/test-words*.idx
	rm -fd test-index-files
