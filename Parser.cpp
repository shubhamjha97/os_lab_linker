#include <map>
#include <iomanip>
#include "Tokenizer.cpp"

class Parser {

    static const int DEFCOUNT_LIMIT = 16;
    static const int USECOUNT_LIMIT = 16;
    static const int MACHINE_SIZE = 512;

    int memoryMapPtr = 0;
    string tokenBuffer;
    Tokenizer tokenizer;
    vector<string> useList, symbolDefinitionOrderList;

    map<string, bool> usedSymbols;
    map<string, bool> globalUsedSymbols;
    map<string, int> symbolDefinitionLocation;
    map<int, int> baseAddresses;

    vector<vector<string> > moduleUseLists;
    map<string, int> symbolTable;
    int moduleBaseAddress;
    int globalAddress;
    int currentModuleCount;
    vector<int> memoryMap;
    vector<string> programErrors;
    map<string, string> symbolErrors;
    vector<int> moduleSizes;

public:
    explicit Parser(ifstream & inFile) : tokenizer(inFile) {
        moduleBaseAddress = 0;
        globalAddress = 0;
        currentModuleCount = 0;
    }

    void clearState() {
        moduleBaseAddress = 0;
        globalAddress = 0;
        currentModuleCount = 0;
        memoryMapPtr = 0;
        tokenizer.clearState();
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
                return false;
            }

            if(pass1) {
                // If symbol doesn't already exist in table
                if(symbolTable.find(symbol) == symbolTable.end()) {
                    symbolTable[symbol] = moduleBaseAddress + addr;
                    symbolDefinitionOrderList.push_back(symbol);
                    symbolDefinitionLocation[symbol] = currentModuleCount;
                } else {
                    symbolErrors[symbol] = "Error: This variable is multiple times defined; first value used"; // Rule 2
                }
            }
        }
        return true;
    }

    bool readUseList(bool pass1) {
        int useCount = 0;
        useList.clear();
        if(!tokenizer.readInteger(useCount)) {
            tokenizer.parseErrorAndExit(0);
            return false;
        }

        if(useCount > USECOUNT_LIMIT) {
            tokenizer.parseErrorAndExit(5);
        }

        while(useCount--) {
            if(!tokenizer.getNextToken(tokenBuffer) || !tokenizer.isValidSymbol(tokenBuffer)) {
                tokenizer.parseErrorAndExit(1);
                return false;
            }
            if(pass1) {
                useList.push_back(tokenBuffer);
            }
        }
        moduleUseLists.push_back(useList);
        usedSymbols.clear();
        return true;
    }

    bool readProgramText(bool pass1) {
        int codeCount = 0;
        if(!tokenizer.readInteger(codeCount)){
            tokenizer.parseErrorAndExit(0);
            return false;
        }

        if(pass1 && (codeCount + globalAddress - 1 >= MACHINE_SIZE)) {
            tokenizer.parseErrorAndExit(6);
        }

        while(codeCount--) {
            char opType;
            int instr = 0;
            if(!tokenizer.readOpType(opType) || !tokenizer.readInstr(instr)) {
                tokenizer.parseErrorAndExit(0);
                return false;
            }

            int operand = Tokenizer::getOperand(instr);
            int opcode = Tokenizer::getOpcode(instr);

            if(!pass1) {
                int addr = operand;
                string symbol;
                string error = "";
                switch (opType) {
                    case 'R': // Relative
                        addr = operand + moduleBaseAddress;
                        if(operand > moduleSizes[currentModuleCount]) {
                            addr = moduleBaseAddress; // Use relative 0 address i.e. the module base address
                            error = "Error: Relative address exceeds module size; zero used"; // Rule 9
                        }
                        break;
                    case 'E': // External
                        if(operand >= moduleUseLists[currentModuleCount].size()) {
                            addr = operand; // Treat as immediate
                            error = "Error: External address exceeds length of uselist; treated as immediate"; // Rule 6
                            break;
                        }
                        symbol = moduleUseLists[currentModuleCount][operand];
                        if(symbolTable.find(symbol) != symbolTable.end()) {
                            addr = symbolTable[symbol];
                        } else {
                            error = "Error: " + symbol + " is not defined; zero used"; // Rule 3
                            addr = 0;
                        }
                        usedSymbols[symbol] = true; // Mark symbol as used in the current module
                        globalUsedSymbols[symbol] = true; // Mark symbol as used globally
                        break;
                    case 'I': // Immediate
                        addr = operand;
                        if(instr >= 10000) {
                            // Set instr to 9999
                            opcode = 9;
                            addr = 999;
                            error = "Error: Illegal immediate value; treated as 9999"; // Rule 10
                        }
                        break;
                    case 'A': // Absolute
                        addr = operand;
                        if(addr > MACHINE_SIZE) {
                            addr = 0;
                            error = "Error: Absolute address exceeds machine size; zero used"; // Rule 8
                        }
                        break;
                }

                if(opcode >=10 ) {
                    error = "Error: Illegal opcode; treated as 9999"; // Rule 11
                    // Set instr to 9999
                    opcode = 9;
                    addr = 999;
                }

                memoryMap.push_back(opcode*1000 + addr);
                programErrors.push_back(error);
            }

            globalAddress++;
        }
        return true;
    }

    bool readModule(bool pass1=true) {
        moduleBaseAddress = globalAddress;
        if(pass1){
            baseAddresses[currentModuleCount] = moduleBaseAddress;
        }
        if(!readDefinitionList(pass1)) {
            return false;
        }
        if(!readUseList(pass1) || !readProgramText(pass1)) {
            return false;
        }
        if(pass1) {
            moduleSizes.push_back(globalAddress - moduleBaseAddress);
        }
        currentModuleCount++;
        return true;
    }

    void checkIfAllDefinedModulesUsed() { // Rule 4
        string warning, symbol;
        int definitionLocation;

        for (auto symbol : symbolDefinitionOrderList) {
            definitionLocation = symbolDefinitionLocation[symbol];
            if(!globalUsedSymbols[symbol]) {
                warning = "Warning: Module " + to_string(definitionLocation+1)
                        + ": " + symbol + " was defined but never used";
                cout<<warning<<endl;
            }
        }
    }

    void checkIfAllUseListModulesUsed() { // Rule 7
        string warning;
        for (auto symbol : moduleUseLists[currentModuleCount-1]) {
            if(!usedSymbols[symbol]) {
                warning = "Warning: Module "+ to_string(currentModuleCount) + ": " + symbol
                        + " appeared in the uselist but was not actually used";
                cout<<warning<<endl;
            }
        }
    }

    void checkIfDefinitionInModuleBounds() { // Rule 5
        string warning;
        for(auto symbol : symbolDefinitionOrderList) {
            int definitionModule = symbolDefinitionLocation[symbol];
            int moduleBaseAddress = baseAddresses[definitionModule];
            int moduleBound = moduleSizes[definitionModule] - 1;
            int symbolAbsoluteAddress = symbolTable[symbol];

            if( symbolAbsoluteAddress - moduleBaseAddress > moduleBound) {
                warning = "Warning: Module " + to_string(definitionModule+1) + ": " + symbol
                        + " too big " + to_string(symbolTable[symbol] - moduleBaseAddress) + " (max="
                        + to_string(moduleBound) + ") assume zero relative";
                cout<<warning<<endl;
                symbolTable[symbol] = moduleBaseAddress; // Use the base address of the module (Rule 5)
            }
        }
    }

    void printSymbolTable() {
        cout<<"Symbol Table"<<endl;
        for(auto symbol : symbolDefinitionOrderList ) {
            cout<<symbol<<"="<<symbolTable[symbol];
            if(symbolErrors.find(symbol) != symbolErrors.end()) {
                cout<<" "<<symbolErrors[symbol]; // Print errors related to the symbol
            }
            cout<<endl;
        }
    }

    void printMemoryMap() {
        int n = memoryMap.size();
        while(memoryMapPtr < n) {
            cout<<setfill('0')<<setw(3)<<memoryMapPtr<<": ";
            cout<<setfill('0')<<setw(4)<<memoryMap[memoryMapPtr];
            if(!programErrors[memoryMapPtr].empty()) {
                cout<<" "<<programErrors[memoryMapPtr];
            }
            cout<<endl;

            memoryMapPtr++;
        }
    }

    // Pass 1
    void runPass1() {
        while(readModule(true));
    }

    // Pass 2
    void runPass2() {
        // Move tokenizer to beginning of file
        tokenizer.seekToBeginning();
        clearState();

        // Check if symbol addresses are within module bounds (Rule 5)
        checkIfDefinitionInModuleBounds();

        // Print symbol table
        printSymbolTable();
        cout<<endl;

        // Print Memory Map
        cout<<"Memory Map"<<endl;
        while(readModule(false)) {
            printMemoryMap();
            // Check if all modules defined in use list are actually used in module (Rule 7)
            checkIfAllUseListModulesUsed();
        }
        cout<<endl;

        // Check if all defined symbols are used in some module (Rule 4)
        checkIfAllDefinedModulesUsed();
        cout<<endl;
    }
};
