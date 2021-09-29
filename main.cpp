#include<iostream>
#include "Parser.cpp"

using namespace std;

int main(int argc, char *argv[]) {
    string filename = "lab1_assign/"+(string) argv[1];

    ifstream inFile(filename);
    Parser parser = Parser(inFile);
    parser.runPass1();
    parser.runPass2();
    return 0;
}
