#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>

using namespace std;

string gen_randomText(int len) {
	string s;
	string alphanum = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-_=+[]{}|;:\'\"<>,.?/`~";


	for (int i  = 0; i < len; i++){
		s += alphanum[rand() % sizeof(alphanum)];
	}


	return s;

}
/*
char* gen_randomBin(int len){
	char *s[len];
	const char nibbles[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

	for(int i = 0; i < len; i++){
		s[i] = nibbles[(rand() % 16)] ^ (nibbles[rand() % 16] << 4);
	}

	s[len] = 0x00;

	return s;
}
*/
string textSigGen(int len){

        const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-_=+[]{}|;:<>,.?/`~";

        string sig = "\"";

        for(int i = 1; i < len-1; i++){
                sig += alphanum[rand() % (sizeof(alphanum)-1)];
        }

	sig += "\"";
        sig[len] = 0x00;

	//cout << sig << endl;
	
	return sig;

}
/*
char* binSigGen(int len){
        char *sig[len];

        const char nibbles[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

        sig[0] = '"';

        for(int i = 0; i< len; i++){
                sig[i] = nibbles[(rand() % 16)] ^ (nibbles[rand() % 16] << 4);
        }

        sig[len-1] = '"';
        sig[len] = 0x00;

        return sig;

}
*/

string gen_rule(string ruleid, string sig) {

	string rule = "rule test" + ruleid + " { strings: $my_text_string = ";

	rule += sig;
	rule += " condition: $my_text_string }\n";

	return rule;

}

int main(int argc, char *argv[]){

	printf("%d\n", argc);

	if(argc != 5){
		printf("Useage <num signatures> <length of signatures> <number of test files> <len test files>");
		exit(1);
	}

	int numSig = atoi(argv[1]);
	int sigLen = atoi(argv[2]);
	int numFiles = atoi(argv[3]);
	int fileLen = atoi(argv[4]) - sigLen;

	/*initalize vector of stirngs*/
	vector<string> signatures;


	/*initlize vecotr of rules */
	vector<string> rules;


	/*generate signatures and rules */
	for(int i = 0; i < numSig; i++){
		string sig = textSigGen(sigLen);
		string ruleid = to_string(i);
		string rule = gen_rule(ruleid, sig);
		signatures.push_back(sig);
		rules.push_back(rule);
	}

	/*open rule file and write rules */
	ofstream rulefile;
	rulefile.open("yaraRules");
	for(int i = 0; i < 10; i++){
		rulefile << rules[i] << endl;
	}
	rulefile.close();


	/*make test directory to put testfiles in */
	if(mkdir("testDir", ACCESSPERMS) != 0){
		printf("Could not make directory");
	}

	ofstream record;
	record.open("record.txt");



	for(int i = 0; i < numFiles; i++){
		/* get file read */
		string testFileName = "testDir/testFile";
		testFileName += to_string(i);
		ofstream testFile;
		testFile.open(testFileName);

		/* get offset ready */
		int diff = rand() % fileLen;
		int firstlen = fileLen - diff;

		/*generate text file string */
		string first = gen_randomText(firstlen);
		string second = gen_randomText(diff);
		int sigIndex = (rand() % signatures.size());
		string sig = signatures[rand() % signatures.size()];
		string filetext = first + sig + second;

		record << testFileName + " : " + sig + " (rule " + to_string(sigIndex) + " ) \n";

		/*write text file string to file and close file */
		testFile << filetext;
		testFile.close();
	}

	record.close();
	return 0;

}
