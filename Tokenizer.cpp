#include <iostream>
#include<string>
#include<string.h>
#include<vector>

using namespace std;

class Tokenizer {
    string currentLine;
    int currentLineNumber;
    int currentTokenIdx;
    vector<string> tokens;
    const char* delimiters = " \t\n";

public:
    Tokenizer() {
        currentTokenIdx = 0;
        currentLineNumber = 0;
        readLine();
    }

    void tokenizeLine(string line) {
        char* token;
        token = strtok(&line[0], delimiters);
        while (token != NULL)
        {
            tokens.push_back(string(token));
            token = strtok (NULL, delimiters);
        }
    }

    bool readLine() {
        tokens.clear();
        if(std::getline (std::cin, currentLine)) {
            currentLineNumber++;
            currentTokenIdx = 0;
            tokenizeLine(currentLine);
            return true;
        }
        return false;
    }

    bool getNextToken(string &tokenBuffer){
        tokenBuffer.clear();
        if(currentTokenIdx == tokens.size()) {
            if(!readLine()) {
                return false;
            }
        }
        tokenBuffer.append(tokens[currentTokenIdx]);
        currentTokenIdx++;
        return true;
    }

    static bool isInteger(string token) {
        // TODO: Implement
        return true;
    }

    int readInteger(int &intBuffer) {
        string tokenBuffer;
        if(!getNextToken(tokenBuffer) || !isInteger(tokenBuffer)) {
            return false;
        }
        intBuffer = (int)stoi(tokenBuffer);
        return true;
    }
};
