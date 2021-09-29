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
    int currentLineOffset;
    vector<string> tokens;
    const char* delimiters = " \t\n";
    ifstream &inFile;

public:
    Tokenizer(ifstream & in) : inFile(in) {
        currentTokenIdx = 0;
        currentLineNumber = 0;
        currentLineOffset = 0; // TODO: increment this properly
    }

    void clearState() {
        tokenBuffer.clear();
        tokens.clear();
        currentLine.clear();

        currentLineNumber = 0;
        currentTokenIdx = 0;
        currentLineOffset = 0;
    }

    void seekToBeginning() {
        inFile.clear();
        inFile.seekg(0);
    }

    void parseErrorAndExit(int errorcode) {
        __parseerror(errorcode);
        exit(EXIT_FAILURE);
    }

    void __parseerror(int errcode) {
        static char* errstr[] = {
                "NUM_EXPECTED",             // Number expect, anything >= 2^30 is not a number either
                "SYM_EXPECTED",             // Symbol Expected
                "ADDR_EXPECTED",            // Addressing Expected which is A/E/I/R
                "SYM_TOO_LONG",             // Symbol Name is too long
                "TOO_MANY_DEF_IN_MODULE",   // >16
                "TOO_MANY_USE_IN_MODULE",   // > 16
                "TOO_MANY_INSTR"            // total num_instr exceeds memory size (512)
        };
        printf("Parse Error line %d offset %d: %s\n", currentLineNumber, currentLineOffset, errstr[errcode]);
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
        while(tokens.empty() && !inFile.eof()) {
            if(!getline (inFile, currentLine)) {
                return false;
            }
            currentLineNumber++;
            currentTokenIdx = 0;
            tokenizeLine(currentLine);
        }
        cout<<"Read line: "<<currentLine<<endl;
        return !tokens.empty();
    }

    bool getNextToken(string &tokenBuffer){
        tokenBuffer.clear();
        if(currentTokenIdx == tokens.size()) {
            if(!readLine()) {
                return false;
            }
        }
        tokenBuffer.append(tokens[currentTokenIdx]);
        cout<<"Token: "<<tokenBuffer<<endl; // TODO: remove
        currentTokenIdx++;
        return true;
    }

    bool isValidSymbol(string token) {
        int symbolSize = token.size();
        if(symbolSize > 16) {
            parseErrorAndExit(3);
            return false;
        }
        if(symbolSize==0 || !isalpha(token[0])) {
            return false;
        }
        for(int i=1; i<symbolSize; i++) {
            if(!isalnum(token[i])) {
                return false;
            }
        }
        return true;
    }

    bool isValidInteger(string token) {
        // TODO: Implement
        return true;
    }

    bool isValidOpType(string token) {
        if(token.empty()
            || !(token[0] == 'R' || token[0] == 'E' || token[0] == 'I' || token[0] == 'A')) {
            parseErrorAndExit(2);
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
            parseErrorAndExit(2);
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
        if(!getNextToken(tokenBuffer) || !isValidInteger(tokenBuffer)) {
            return false;
        }
        intBuffer = (int)stoi(tokenBuffer);
        return true;
    }

    bool readSymbol(string &symbolBuffer) {
        if(!getNextToken(symbolBuffer) || !isValidSymbol(symbolBuffer)){
            parseErrorAndExit(1);
            return false;
        }
        return true;
    }
};
