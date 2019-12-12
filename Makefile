all: textGen

textGen:
	g++ -g -o textFileGen textFileGen.cpp

clean:
	rm -r testDir
