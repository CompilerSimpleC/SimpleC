#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum term{vtype, id, semi, assign, literal, character, boolstr, addsub, multdiv, lparen, rparen, num, lbrace, rbrace, comma, iff, whilee, comp, elsee, returnn, dollor};
enum nonterm{CODE, VDECL, ASSIGN, RHS, EXPR, EXPT, TERM, FACTOR, FDECL, ARG, MOREARGS, BLOCK, STMT, COND, COND_, ELSE, RETURN};

pair<char, int> ACTION[74][21];
int G0T0[74][16];

void InitializeGOTO()
{
    G0T0[0][VDECL] = 1;
    G0T0[1][CODE] = 3;
    G0T0[1][VDECL] = 1;
    G0T0[1][FDECL] = 4;
    G0T0[2][ASSIGN] = 7;
    G0T0[4][CODE] = 8;
    G0T0[4][VDECL] = 1;
    G0T0[4][FDECL] = 4;
    G0T0[5][ASSIGN] = 7;
    G0T0[11][RHS] = 14;
    G0T0[11][EXPR] = 15;
    G0T0[11][TERM] = 19;
    G0T0[11][FACTOR] = 21;
    G0T0[13][ARG] = 24;
    G0T0[20][EXPR] = 28;
    G0T0[20][TERM] = 19;
    G0T0[20][FACTOR] = 21;
    G0T0[26][TERM] = 31;
    G0T0[26][FACTOR] = 21;
    G0T0[27][FACTOR] = 32;
    G0T0[30][MOREARGS] = 35;
    G0T0[34][VDECL] = 39;
    G0T0[34][ASSIGN] = 40;
    G0T0[34][BLOCK] = 37;
    G0T0[34][STMT] = 38;
    G0T0[37][RETURN] = 45;
    G0T0[38][VDECL] = 39;
    G0T0[38][ASSIGN] = 40;
    G0T0[38][BLOCK] = 47;
    G0T0[38][STMT] = 38;
    G0T0[46][RHS] = 53;
    G0T0[46][EXPR] = 15;
    G0T0[46][TERM] = 19;
    G0T0[46][FACTOR] = 21;
    G0T0[49][COND] = 54;
    G0T0[50][COND] = 56;
    G0T0[51][MOREARGS] = 57;
    G0T0[62][VDECL] = 39;
    G0T0[62][ASSIGN] = 40;
    G0T0[62][BLOCK] = 65;
    G0T0[62][STMT] = 38;
    G0T0[64][VDECL] = 39;
    G0T0[64][ASSIGN] = 40;
    G0T0[64][BLOCK] = 66;
    G0T0[64][STMT] = 38;
    G0T0[67][ELSE] = 69;
    G0T0[71][VDECL] = 39;
    G0T0[71][ASSIGN] = 40;
    G0T0[71][BLOCK] = 72;
    G0T0[71][STMT] = 38;
}

int main(int argc, char* argv[]){
    // input이 없는 경우
    if(argc == 1){
        cout << "Error: There's no argument of main function.";
        return 0;
    }
    
    // input이 여러 개 있는 경우
    if(argc > 2){
        cout << "Error: There are too many argument of main function.";
        return 0;
    }

    ifstream myfile;
    string s = argv[1];
    myfile.open(s);

    // input file이 존재하지 않는 경우
    if(!myfile.is_open()){
        cout << "Error: There's no such file with name '" << s << "'" << endl;
        return 0;
    }
    
    string input;
    getline(myfile, input);

    // token 넣기
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
    
    //for(vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++){
    //    cout << (*it) << endl;
    //}

    return 0;
}
