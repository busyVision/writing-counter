#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

bool isAlphabet(char c) {
	return '!' <= c && c <= '~';
}

class Paragraph {
public:
	int chars, words, sentences;
	string firstWord, lastWord;
	Paragraph(int c, int w, int s, string f, string l) : chars(c), words(w), sentences(s), firstWord(f), lastWord(l) {}
};

class Counter {
protected:
	char read;
	bool inWord, inBlank, sentenceEndMet, paragraphJustEnded;
	int lineTemp, spaceTemp;
	string lastWord, firstInThis;
public:
	int charCount, wordCount, sentenceCount, paraCount, paraCharCount, paraWordCount, paraSentenceCount;
	bool countSpaces = false, countSentenceEnds = false;
	vector<Paragraph> paragraphs; //vector to store paraCharCounts, paraWordCounts and paraSentenceCounts
	void initialize() {
		inWord = sentenceEndMet = false;
		paragraphJustEnded = inBlank = true;
		lineTemp = spaceTemp = charCount = wordCount = sentenceCount = paraCount = paraCharCount = paraWordCount = paraSentenceCount = 0;
		paragraphs.clear();
	}
	void count(ifstream& file) {
		initialize();
		while (file.good()) {
			read = file.get();
			switch (read) {
			case '\n':
				lineTemp++;
				goto BLANK; //Placed to avoid incrementing paraCharCount when '\n' is detected. TODO: Use a more elegant method if there is one
			case ' ':
				if (countSpaces) paraCharCount++;
			case '\t':
			BLANK:
				if (inWord) {
					flipState();
					if (sentenceEndMet) paraSentenceCount++;
				}
				break;
			case '.':case '?':case '!':
				sentenceEndMet = true;
				if (countSentenceEnds) paraCharCount++;
				continue; //instead of break so as to avoid sentenceEndMet being set to false
			default:
				//if (isAlphabet(read)) {
					if (inBlank) { //another word encountered
						if (lineTemp >= 2) { //new paragraph started
							paragraphEnd();
							paragraphJustEnded = true;
						}
						else if (paragraphJustEnded && paraWordCount > 0) { //The first word of a new paragraph. Check paraWordCount so it doesn't register an empty string as the first word of the first paragraph
							firstInThis = lastWord;
							paragraphJustEnded = false;
						}
						lastWord.clear(); //Before paragraphEnd() since the method uses the field
						lineTemp = 0;
						paraWordCount++;
						flipState();
					}
					paraCharCount++;
					lastWord += read;
				//}
				break;
			}
			sentenceEndMet = false;
		}
		//The last paragraph and sentence aren't counted in above loop
		paraSentenceCount++;
		paragraphEnd();
	}
	void flipState() {
		inWord = !inWord;
		inBlank = !inBlank;
	}
	void paragraphEnd() {
		paraCount++;
		paragraphs.push_back(Paragraph(paraCharCount, paraWordCount, paraSentenceCount, firstInThis, lastWord));
		charCount += paraCharCount;
		wordCount += paraWordCount;
		sentenceCount += paraSentenceCount;
		paraCharCount = paraWordCount = paraSentenceCount = 0;
	}
};
void printHelp();
int main(int argc, char** argv) {
	if (argc <= 1) { //The file name wasn't supplied
		printHelp();
		return 0;
	}
	//Parse and apply arguments
	Counter c;
	bool showParaWords = false;;
	for (int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) c.countSpaces = true;
		else if (strcmp(argv[i], "-p") == 0) c.countSentenceEnds = true;
		else if (strcmp(argv[i], "-w") == 0) showParaWords = true;
	}
	ifstream in(argv[1]);
	//Analyze the file
	c.count(in);
	//Print results 
	cout << c.charCount << " characters, " << c.wordCount << " words, " << c.sentenceCount << " sentences, " << c.paraCount << " paragraphs\n";
	for (int i = 0; i < c.paraCount; i++) {
		cout << "Paragraph " << (i + 1) << " has " << c.paragraphs[i].sentences << " sentences, " << c.paragraphs[i].words << " words, " << c.paragraphs[i].chars << " characters\n";
		if (showParaWords) cout << '(' << c.paragraphs[i].firstWord << " ... " << c.paragraphs[i].lastWord << ")\n";
	}
	in.close();
	return 0;
}
void printHelp() {
	cout << "Use : count filename [-s] [-p] [-w]\n";
	cout << "Counts(from an ASCII file):\n";
	cout << "number of pharagraphs, \n";
	cout << "number of characters, words and sentences(in entire file and in each paragraphs).\n";
	cout << "-s : count spaces as characters.\n";
	cout << "-p : count punctuation marks that end a sentence(., ?, !) as characters.\n";
	cout << "-w : show the first and last ewords of each paragraph.\n";
	cout << "A paragraph is a group of words preceded by two newlines or the start of the file.\n";
}