#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

//enum term{vtype, id, semi, assign, literal, character, boolstr, addsub, multdiv, lparen, rparen, num, lbrace, rbrace, comma, if, while, comp, else, return, dollor};
//enum nonterm{CODE, VDECL, ASSIGN, RHS, EXPR, EXPR_, FDECL, ARG, MOREARGS, BLOCK, STMT, COND, COND_, ELSE, RETURN};

int main(int argc, char* argv[]){
    
    if(argc == 1){
        cout << "Error: There's no argument of main function.";
        return 0;
    }

    if(argc > 2){
        cout << "Error: There are too many argument of main function.";
        return 0;
    }

    ifstream myfile;
    string s = argv[1];
    myfile.open(s);

    if(!myfile.is_open()){
        cout << "Error: There's no such file with name '" << s << "'" << endl;
        return 0;
    }
    
    string input;
    getline(myfile, input);

    vector<string> tokens;
    string temp = "";
    for(int i = 0 ; i < input.length(); i++){
        if(input[i] == ' '){
            if(temp != "") {
                tokens.push_back(temp);
                temp = "";
            }
        }
        else if(i == input.length() - 1){
            if(input[i] != ' '){
                temp += input[i];
            }
            tokens.push_back(temp);
        }
        else temp += input[i];
    } 

    for(vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++){
        cout << (*it) << endl;
    }

    return 0;
}
