#include <map>
#include "Tokenizer.cpp";

class Parser {
    string tokenBuffer;
    Tokenizer tokenizer;
    vector<string> useList, symbolDefinitionOrderList;
    map<string, int> symbolTable;
    int moduleBaseAddress;
    int globalAddress;

public:

    Parser() {
        moduleBaseAddress = 0;
        globalAddress = 0;
    }

    void thwowParseError() {
        // TODO: implement method which uses the current context to throw an exception
    }

    void warn() {
        // TODO: implement method which prints a warning
    }

    bool readDefinitionList() {
        int defCount = 0;
        string symbol;
        int addr;
        if(!tokenizer.readInteger(defCount)) {
            return false;
        }

        while(defCount--) {
            if(!tokenizer.readSymbol(tokenBuffer)) {
                return false;
            }
            symbol = tokenBuffer;
            if(!tokenizer.readInteger(addr)) {
                // TODO: Check if any validation is needed here
                return false;
            }

            symbolTable[symbol] = moduleBaseAddress + addr;
            symbolDefinitionOrderList.push_back(symbol);
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
            char opType;
            int instr = 0;
            if(!tokenizer.readOpType(opType) || !tokenizer.readInstr(instr)) {
                return false;
            }

            int operand = tokenizer.getOperand(instr), addr = operand;

            switch (opType) {
                case 'R':
                    // TODO: Calculate moduleBaseAddress
                    addr = operand + moduleBaseAddress;
                    break;
                case 'E':
                    // TODO: Get relative address of symbols defined in the useList
                    // TODO: Symbols can even be defined later. Figure out how to do this.
                    break;
                case 'I':
                    addr = operand;
                    break;
                case 'A':
                    addr = operand;
                    // TODO: Throw error when operand >= 512 (machine size)
                    break;
                // TODO: Maybe, for all cases, ensure that address doesn't go >= 512
            }

            globalAddress++;
        }
        return true;
    }

    bool processProgramText() {
        // TODO: Logic for pass2.
        return false;
    }

    bool readModule() {
        moduleBaseAddress = globalAddress;
        // TODO: Debug
        if(!readDefinitionList() || !readUseList() || !readProgramText()) {
            return false;
        }
        cout<<"read module with base address: "<<moduleBaseAddress<<endl;
        return true;
    }

    void printSymbolTable() {
        cout<<"Symbol Table"<<endl;
        for(auto symbol : symbolDefinitionOrderList ) {
            // Print k, v
            cout<<symbol<<"="<<symbolTable[symbol]<<endl;
        }
    }

    void printMemoryMap() {
        return;
    }

    void runPass1() {
        while(readModule());
        printSymbolTable();
    }

    void runPass2() {
        while(readModule());
        printMemoryMap();
    }
};
