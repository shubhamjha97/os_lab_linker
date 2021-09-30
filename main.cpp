#include<iostream>
#include "Parser.cpp"

using namespace std;

int main(int argc, char *argv[]) {
    string filename = "lab1_assign/"+(string) argv[1]; // Take filename from first arg
    ifstream inFile(filename);

    Parser parser = Parser(inFile);
    parser.runPass1(); // Run first pass
    parser.runPass2(); // Run second pass

    return 0;
}
