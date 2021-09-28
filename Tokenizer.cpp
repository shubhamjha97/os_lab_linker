//
// Created by Shubham Jha on 9/28/21.
//

#include "Tokenizer.h"
#include <iostream>
#include<string>
using namespace std;

// TODO: Read lines from stdin
// TODO: Read file line by line, keep track of current line, print current line

class Tokenizer {
    // TODO: Code tokenzier
    // TODO: create a main class
    // TODO: Write Makefile
    // TODO: Add sample input
    string currentLine;

public:
    char *pch;

    string getToken() {
        std::getline(std::cin, currentLine);

        char *dup = strdup(currentLine.c_str());
        pch = strtok (dup," ,.-");
        while (pch != NULL)
        {
            cout<<pch<<endl;
            pch = strtok(NULL, " ,.-");
        }
        free(dup);
        return "Done";
    }
};

int main() {
    Tokenizer tokenizer = Tokenizer();
    for(int i=0; i<10; i++) {
        cout<<tokenizer.getToken()<<endl;
    }
    cout<<"DONE"<<endl;
    return 0;
}
