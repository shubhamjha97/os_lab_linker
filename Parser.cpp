#include <map>
#include "Tokenizer.cpp";

class Parser {
    Tokenizer tokenizer;
    map<string, int> definitionList;
    vector<string> useList;
    string tokenBuffer;

public:

    bool readDefinitionList() {
        int defCount = 0;
        string symbol;
        int addr;
        tokenizer.readInteger(defCount);

        while(defCount--) {
            if(!tokenizer.getNextToken(tokenBuffer)) {
                return false;
            }
            symbol = tokenBuffer;
            tokenizer.readInteger(addr);
            definitionList[symbol] = addr;
        }
        return true;
    }

    bool readUseList() {
        int useCount = 0;
        if(!tokenizer.readInteger(useCount)) {
            return false;
        };

        while(useCount--) {
            if(!tokenizer.getNextToken(tokenBuffer)) {
                return false;
            }
            useList.push_back(tokenBuffer);
        }
        return true;
    }

    bool readProgramText() {
        int codeCount = 0;
        if(!tokenizer.readInteger(codeCount)){
            return false;
        }
        while(codeCount--) {
            // TODO:
        }
        // TODO: Return false on failure
        return true;
    }

    bool readModule() {
        if(!readDefinitionList() || !readUseList() || !readProgramText) {
            return false;
        }
        return true;
    }

    void runPass1() {
        while(readModule());
        return;
    }

    void runPass2() {
        cout<<"Pass 2 not implemented"<<endl;
        return;
    }
};