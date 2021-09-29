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
    Parser(ifstream & inFile) : tokenizer(inFile) {
        moduleBaseAddress = 0;
        globalAddress = 0;
    }

    void thwowParseError() {
        // TODO: implement method which uses the current context to throw an exception
    }

    void warn() {
        // TODO: implement method which prints a warning
    }

    bool readDefinitionList(bool pass1) {
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

            if(pass1) {
                symbolTable[symbol] = moduleBaseAddress + addr;
                symbolDefinitionOrderList.push_back(symbol);
            }
        }
        return true;
    }

    bool readUseList(bool pass1) {
        int useCount = 0;
        if(!tokenizer.readInteger(useCount)) {
            return false;
        };

        while(useCount--) {
            if(!tokenizer.getNextToken(tokenBuffer)) {
                return false;
            }
            if(pass1) {
                useList.push_back(tokenBuffer);
            }
        }
        return true;
    }

    bool readProgramText(bool pass1) {
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
            // TODO: Add pass-specific logic

            globalAddress++;
        }
        return true;
    }

    bool processProgramText() {
        // TODO: Logic for pass2.
        return false;
    }

    bool readModule(bool pass1=true) {
        moduleBaseAddress = globalAddress;
        // TODO: Debug
        if(!readDefinitionList(pass1) || !readUseList(pass1) || !readProgramText(pass1)) {
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
        // TODO: implement
        // TODO: Print the memory map and the warnings/errors
        return;
    }

    void runPass1() {
        while(readModule());
        printSymbolTable();
    }

    void runPass2() {
        // TODO: Seek to beginning of stdin
        while(readModule());
        printMemoryMap();
    }
};
