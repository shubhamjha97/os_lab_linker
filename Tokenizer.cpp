#include <iostream>
#include<string>
#include<string.h>
#include<vector>
#include <fstream>

using namespace std;

class Tokenizer {
    string currentLine;
    string tokenBuffer;
    int currentLineNumber;
    int currentTokenIdx;
    vector<string> tokens;
    const char* delimiters = " \t\n";
    ifstream &inFile;

public:
    Tokenizer(ifstream & in) : inFile(in) {
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
        if(getline (inFile, currentLine)) {
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

    static bool isValidOpType(string token) {
        if(token.empty()
            || !(token[0] == 'R' || token[0] == 'E' || token[0] == 'I' || token[0] == 'A')) {
            return false;
        }
        return true;
    }

    static int getOpcode(int instr) {
        return instr/1000;
    }

    static int getOperand(int instr) {
        return instr%1000;
    }

    static bool isValidInstr(string token) {
        // TODO: Add exception handling for all stoi
        int instr = stoi(token);
        if(getOpcode(instr)>=10) {
            return false;
        }
        return true;
    }

    bool readOpType(char &opBuffer) {
        if(!getNextToken(tokenBuffer) || !isValidOpType(tokenBuffer)) {
            return false;
        }
        opBuffer = tokenBuffer[0];
        return true;
    }

    bool readInstr(int &instrBuffer) {
        if(!getNextToken(tokenBuffer) || !isValidInstr(tokenBuffer)) {
            return false;
        }
        instrBuffer = stoi(tokenBuffer);
        return true;
    }

    bool readInteger(int &intBuffer) {
        if(!getNextToken(tokenBuffer) || !isInteger(tokenBuffer)) {
            return false;
        }
        intBuffer = (int)stoi(tokenBuffer);
        return true;
    }

    bool readSymbol(string &symbolBuffer) {
        // TODO: use this method while reading a symbol
        if(!getNextToken(symbolBuffer)){
            return false;
        }
        return true;
    }
};
