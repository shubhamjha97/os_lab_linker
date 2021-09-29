#include <map>
#include "Tokenizer.cpp";

class Parser {

    static const int DEFCOUNT_LIMIT = 16;
    static const int USECOUNT_LIMIT = 16;
    static const int MEMORY_SIZE = 512;

    string tokenBuffer;
    Tokenizer tokenizer;
    vector<string> useList, symbolDefinitionOrderList;
    map<string, bool> usedSymbols;
    vector<vector<string> > moduleUseLists;
    map<string, int> symbolTable;
    int moduleBaseAddress;
    int globalAddress;
    int currentModuleCount;
    vector<int> memoryMap;

public:
    Parser(ifstream & inFile) : tokenizer(inFile) {
        moduleBaseAddress = 0;
        globalAddress = 0;
        currentModuleCount = 0;
    }

    void clearState() {
        moduleBaseAddress = 0;
        globalAddress = 0;
        currentModuleCount = 0;
        tokenizer.clearState();
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

        if(defCount > DEFCOUNT_LIMIT) {
            tokenizer.parseErrorAndExit(4);
        }

        while(defCount--) {
            if(!tokenizer.readSymbol(tokenBuffer)) {
                return false;
            }
            symbol = tokenBuffer;
            if(!tokenizer.readInteger(addr)) {
                tokenizer.parseErrorAndExit(0);
                // TODO: Check if any validation is needed here
                return false;
            }

            if(pass1) {
                symbolTable[symbol] = moduleBaseAddress + addr;
                symbolDefinitionOrderList.push_back(symbol);
                usedSymbols[symbol] = false;
            }
        }
//        cout<<"read definition list"<<endl; // TODO: remove
        return true;
    }

    bool readUseList(bool pass1) {
        int useCount = 0;
        useList.clear();
        if(!tokenizer.readInteger(useCount)) {
            tokenizer.parseErrorAndExit(0);
            return false;
        };

        if(useCount > USECOUNT_LIMIT) {
            tokenizer.parseErrorAndExit(5);
        }

        while(useCount--) {
            if(!tokenizer.getNextToken(tokenBuffer)) {
                return false;
            }
            if(pass1) {
                useList.push_back(tokenBuffer);
            }
        }
        moduleUseLists.push_back(useList);
//        cout<<"read use list"<<endl; // TODO: remove
        return true;
    }

    bool readProgramText(bool pass1) {
        int codeCount = 0;
        if(!tokenizer.readInteger(codeCount)){
            tokenizer.parseErrorAndExit(0);
            return false;
        }

        if(pass1 && (codeCount + globalAddress - 1 >= MEMORY_SIZE)) {
            tokenizer.parseErrorAndExit(6);
        }

        while(codeCount--) {
            char opType;
            int instr = 0;
            if(!tokenizer.readOpType(opType) || !tokenizer.readInstr(instr)) {
                tokenizer.parseErrorAndExit(0); // TODO: check
                return false;
            }

            int operand = tokenizer.getOperand(instr);
            int opcode = tokenizer.getOpcode(instr);

            if(!pass1) {
                int addr = operand;
                string symbol;
                switch (opType) {
                    case 'R':
                        addr = operand + moduleBaseAddress;
                        break;
                    case 'E':
                        // TODO: Check if any validation is required here
                        symbol = moduleUseLists[currentModuleCount][operand];
                        addr = symbolTable[symbol];
                        usedSymbols[symbol] = true;
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
                memoryMap.push_back(opcode*1000 + addr);
            }

            globalAddress++;
        }
//        cout<<"read program text"<<endl; // TODO: remove
        return true;
    }

    bool readModule(bool pass1=true) {
        moduleBaseAddress = globalAddress;
        if(!readDefinitionList(pass1)) {
            return false;
        }
        if(!readUseList(pass1) || !readProgramText(pass1)) {
            return false;
        }
//        cout<<"read module with base address: "<<moduleBaseAddress<<endl; // TODO: remove
        currentModuleCount++;
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
        int n = memoryMap.size();
        cout<<endl;
        cout<<"Memory Map"<<endl;
        for(int i=0; i<n; i++) {
            cout<<std::setfill('0') << std::setw(3)<<i<<": ";
            cout<<std::setfill('0') << std::setw(4)<<memoryMap[i]<<endl;
        }
    }

    void runPass1() {
        while(readModule(true));
        printSymbolTable();
//        cout<<"pass 1 done"<<endl; // TODO: remove
    }

    void runPass2() {
//        cout<<"starting pass 2"<<endl; // TODO: remove
        tokenizer.seekToBeginning();
        clearState();
        while(readModule(false));
        printMemoryMap();
//        cout<<"pass 2 done"<<endl; // TODO: remove
    }
};
