#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <iostream>
#include <vector>
#include <string>

enum term{vtype, id, semi, assign, literal, character, boolstr, addsub, multdiv, lparen, rparen, num, lbrace, rbrace, comma, iff, whilee, comp, elsee, returnn, dollar};
enum nonterm{CODE_, CODE, VDECL, ASSIGN, RHS, EXPR, TERM, FACTOR, FDECL, ARG, MOREARGS, BLOCK, STMT, COND, COND_, ELSE, RETURN};
using namespace std;

class tree {
public:
    tree(string item) {
        this->item = item;
    }

    string getitem() {
        return this->item;
    }
    vector<tree*> getchilds() {
        return this->childs;
    }

    void setitem(string s) {
        this->item = s;
    }
    void setchilds(vector<tree*> c) {
        this->childs = c;
    }
private:
    string item;
    vector<tree*> childs;
};

bool isRoot = true;
pair<char, int> ACTION[75][21];     // row : state       column : terminals
int G0T0[75][18];                   // row : state       column : non terminals
pair<int, int> reduction[34];       // first : GOTO table의 column값.     second : pop할 개수

void init_reduction();      // CFG 각 derivation에 대한 LHS, pop개수를 지정하는 함수
void InitializeGOTO();      // GOTO table 초기화
void init_ACTION();         // ACTION table 초기화
int find_inttoken(const string s); // token string에 대해 token int로 반환
tree* make_child(const int reduction_num, const vector<tree*> &childs); // CFG 번호로 부모 tree를 만들어서 자식 vector 할당 후 부모 string을 return
void printTree(tree root, int level, vector<bool> st, bool isFinalChild);

int main(int argc, char* argv[]){
    // initialization of tables
    init_ACTION();
    init_reduction();
    InitializeGOTO();

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
    tokens.push_back("dollar");
    
    // SLR Parsing
    stack<int> state_stack;   // state
    stack<tree*> token_stack; // token
    tree* root_tree;

    state_stack.push(0); // stack initialization
    int pointer = 0; // input pointer initialization

    while(!state_stack.empty()){
        string input_data = tokens[pointer];            // right token of splitter 추출
        int int_input_data = find_inttoken(input_data); // token의 int형(enum값)
        int state = state_stack.top();
        pair<char, int> table_value = ACTION[state][int_input_data];
        
        if(table_value.first == 's'){       // shift
            pointer++;                      // splitter 이동
            state_stack.push(table_value.second);   // table에 있는 state push
            
            tree* t = new tree(input_data);
            token_stack.push(t);           // 읽은 token stack에 push(나중에 자식으로 만들기 위함)
        }
        else if(table_value.first == 'r'){  // reduce
            int pop_cnt = reduction[table_value.second].second;     // 해당 CFG derivation의 RHS 개수만큼 pop
            vector<tree*> child_vector;
            stack<tree*> temp;
            for(int i = 0; i < pop_cnt; i++) {
                tree* poped_child = token_stack.top();
                temp.push(poped_child);
                state_stack.pop();
                token_stack.pop();
            }
            while(!temp.empty()){
                child_vector.push_back(temp.top());      // 자식 벡터에 자식 추가
                temp.pop();
            }

            if(child_vector.size() == 0){
                tree* epsilon_tree = new tree("epsilon");
                child_vector.push_back(epsilon_tree);
            }

            tree* parent_tree = make_child(table_value.second, child_vector); // 부모 자식 관계 설정
            token_stack.push(parent_tree); // stack에 부모 string push
            int push_value = G0T0[state_stack.top()][reduction[table_value.second].first]; // stack에 남은 state의 top과 derivation의 LHS의 GOTO table value
            state_stack.push(push_value);       // GOTO table value를 state stack에 push
        }
        else if(table_value.first == 'a'){  // accept
            //자식 vector 설정, token stack에 있는 모든 string을 자식으로 만듦
            vector<tree*> child_vector;
            stack<tree*> temp;
            while(!token_stack.empty()){    
                tree* child_tree = token_stack.top();
                temp.push(child_tree);
                token_stack.pop();
            }
            while(!temp.empty()){
                child_vector.push_back(temp.top());
                temp.pop();
            }

            // 부모 tree 만들고 자식vector 할당
            root_tree = new tree("CODE'");
            root_tree->setchilds(child_vector);
            break;
        }
        else {
            // 에러처리. 파싱에러
            cout << "Error: Token <" << input_data <<  "> not matched with ACTION table at [" << state_stack.top() << ", " << input_data << "]" << endl;
            return 0;
        }
    }
    
    // representation of parse tree
    vector<bool> stickStateStack;
    printTree(*root_tree, 0, stickStateStack, true);
    return 0;
}

void init_reduction(){
    reduction[0].first = CODE_; reduction[0].second = 1;        // CODE' -> CODE
    reduction[1].first = CODE; reduction[1].second = 2;         // CODE -> VDECL CODE
    reduction[2].first = CODE; reduction[2].second = 2;         // CODE -> FDECL CODE 
    reduction[3].first = CODE; reduction[3].second = 0;         // CODE -> epsilon
    reduction[4].first = VDECL; reduction[4].second = 3;        // VDECL -> vtype id semi
    reduction[5].first = VDECL; reduction[5].second = 3;        // VDECL -> vtype ASSIGN semi
    reduction[6].first = ASSIGN; reduction[6].second = 3;       // ASSIGN -> id assign RHS
    reduction[7].first = RHS; reduction[7].second = 1;          // RHS -> EXPR
    reduction[8].first = RHS; reduction[8].second = 1;          // RHS -> literal
    reduction[9].first = RHS; reduction[9].second = 1;          // RHS -> character
    reduction[10].first = RHS; reduction[10].second = 1;          // RHS -> boolstr
    reduction[11].first = EXPR; reduction[11].second = 3;       // EXPR -> EXPR addsub TERM
    reduction[12].first = EXPR; reduction[12].second = 1;       // EXPR -> TERM
    reduction[13].first = EXPR; reduction[13].second = 3;       // EXPR -> lparen EXPR rparen
    reduction[14].first = TERM; reduction[14].second = 3;       // TERM -> TERM multdiv FACTOR
    reduction[15].first = TERM; reduction[15].second = 1;       // TERM -> FACTOR
    reduction[16].first = FACTOR; reduction[16].second = 1;     // FACTOR -> id
    reduction[17].first = FACTOR; reduction[17].second = 1;     // FACTOR -> num
    reduction[18].first = FDECL; reduction[18].second = 9;      // FDECL -> vtype id lparen ARG rparen lbrace BLOCK RETURN rbrace
    reduction[19].first = ARG; reduction[19].second = 3;        // ARG -> vtype id MOREARGS
    reduction[20].first = ARG; reduction[20].second = 0;        // ARG -> epsilon
    reduction[21].first = MOREARGS; reduction[21].second = 4;   // MOREARGS -> comma vtype id MOREARGS
    reduction[22].first = MOREARGS; reduction[22].second = 0;   // MOREARGS -> epsilon
    reduction[23].first = BLOCK; reduction[23].second = 2;      // BLOCK -> STMT BLOCK
    reduction[24].first = BLOCK; reduction[24].second = 0;      // BLOCK -> epsilon
    reduction[25].first = STMT; reduction[25].second = 1;       // STMT -> VDECL
    reduction[26].first = STMT; reduction[26].second = 2;       // STMT -> ASSIGN semi
    reduction[27].first = STMT; reduction[27].second = 8;       // STMT -> if lparen COND rparen lbrace BLOCK rbrace ELSE
    reduction[28].first = STMT; reduction[28].second = 7;       // STMT -> while lparen COND rparen lbrace BLOCK rbrace
    reduction[29].first = COND; reduction[29].second = 3;       // COND -> COND comp boolstr
    reduction[30].first = COND; reduction[30].second = 1;       // COND -> boolstr
    reduction[31].first = ELSE; reduction[31].second = 4;       // ELSE -> else lbrace BLOCK rbrace
    reduction[32].first = ELSE; reduction[32].second = 0;       // ELSE -> epsilon
    reduction[33].first = RETURN; reduction[33].second = 3;     // RETURN -> return RHS semi
}

void InitializeGOTO(){
    G0T0[0][CODE] = 1;
    G0T0[0][VDECL] = 3;
    G0T0[0][FDECL] = 2;
    G0T0[2][CODE] = 5;
    G0T0[2][VDECL] = 3;
    G0T0[2][FDECL] = 2;
    G0T0[3][CODE] = 6;
    G0T0[3][VDECL] = 3;
    G0T0[3][FDECL] = 2;
    G0T0[4][ASSIGN] = 8;
    G0T0[9][ARG] = 13;
    G0T0[11][RHS] = 15;
    G0T0[11][EXPR] = 16;
    G0T0[11][TERM] = 20;
    G0T0[11][FACTOR] = 22;
    G0T0[21][EXPR] = 29;
    G0T0[21][TERM] = 20;
    G0T0[21][FACTOR] = 22;
    G0T0[26][MOREARGS] = 31;
    G0T0[27][TERM] = 33;
    G0T0[27][FACTOR] = 22;
    G0T0[28][FACTOR] = 34;
    G0T0[30][VDECL] = 38;
    G0T0[30][ASSIGN] = 39;
    G0T0[30][BLOCK] = 36;
    G0T0[30][STMT] = 37;
    G0T0[36][RETURN] = 45;
    G0T0[37][VDECL] = 38;
    G0T0[37][ASSIGN] = 39;
    G0T0[37][BLOCK] = 47;
    G0T0[37][STMT] = 37;
    G0T0[42][ASSIGN] = 8;
    G0T0[46][RHS] = 54;
    G0T0[46][EXPR] = 16;
    G0T0[46][TERM] = 20;
    G0T0[46][FACTOR] = 22;
    G0T0[49][COND] = 55;
    G0T0[50][COND] = 57;
    G0T0[52][MOREARGS] = 58;
    G0T0[63][VDECL] = 38;
    G0T0[63][ASSIGN] = 39;
    G0T0[63][BLOCK] = 66;
    G0T0[63][STMT] = 37;
    G0T0[65][VDECL] = 38;
    G0T0[65][ASSIGN] = 39;
    G0T0[65][BLOCK] = 67;
    G0T0[65][STMT] = 37;
    G0T0[68][ELSE] = 70;
    G0T0[72][VDECL] = 38;
    G0T0[72][ASSIGN] = 39;
    G0T0[72][BLOCK] = 73;
    G0T0[72][STMT] = 37;
}

void init_ACTION(){
    ACTION[0][vtype] = {'s', 4};
    ACTION[0][dollar] = {'r', 3};
    ACTION[1][dollar] = {'a', 0};
    ACTION[2][vtype] = {'s', 4};
    ACTION[2][dollar] = {'r', 3};
    ACTION[3][vtype] = {'s', 4};
    ACTION[3][dollar] = {'r', 3};
    ACTION[4][id] = {'s', 7};
    ACTION[5][dollar] = {'r', 1};
    ACTION[6][dollar] = {'r', 2};
    ACTION[7][semi] = {'s', 10};
    ACTION[7][assign] = {'s', 11};
    ACTION[7][lparen] = {'s', 9};
    ACTION[8][semi] = {'s', 12};
    ACTION[9][vtype] = {'s', 14};
    ACTION[9][rparen] = {'r', 20};
    ACTION[10][vtype] = {'r', 4};
    ACTION[10][id] = {'r', 4};
    ACTION[10][rbrace] = {'r', 4};
    ACTION[10][iff] = {'r', 4};
    ACTION[10][whilee] = {'r', 4};
    ACTION[10][returnn] = {'r', 4};
    ACTION[10][dollar] = {'r', 4};
    ACTION[11][id] = {'s', 23};
    ACTION[11][literal] = {'s', 17};
    ACTION[11][character] = {'s', 18};
    ACTION[11][boolstr] = {'s', 19};
    ACTION[11][lparen] = {'s', 21};
    ACTION[11][num] = {'s', 24};
    ACTION[12][vtype] = {'r', 5};
    ACTION[12][id] = {'r', 5};
    ACTION[12][rbrace] = {'r', 5};
    ACTION[12][iff] = {'r', 5};
    ACTION[12][whilee] = {'r', 5};
    ACTION[12][returnn] = {'r', 5};
    ACTION[12][dollar] = {'r', 5};
    ACTION[13][rparen] = {'s', 25};
    ACTION[14][id] = {'s', 26};
    ACTION[15][semi] = {'r', 6};
    ACTION[16][semi] = {'r', 7};
    ACTION[16][addsub] = {'s', 27};
    ACTION[17][semi] = {'r', 8};
    ACTION[18][semi] = {'r', 9};
    ACTION[19][semi] = {'r', 10};
    ACTION[20][semi] = {'r', 12};
    ACTION[20][addsub] = {'r', 12};
    ACTION[20][rparen] = {'r', 12};
    ACTION[20][multdiv] = {'s', 28};
    ACTION[21][id] = {'s', 23};
    ACTION[21][lparen] = {'s', 21};
    ACTION[21][num] = {'s', 24};
    ACTION[22][semi] = {'r', 15};
    ACTION[22][addsub] = {'r', 15};
    ACTION[22][rparen] = {'r', 15};
    ACTION[22][multdiv] = {'r', 15};
    ACTION[23][semi] = {'r', 16};
    ACTION[23][addsub] = {'r', 16};
    ACTION[23][rparen] = {'r', 16};
    ACTION[23][multdiv] = {'r', 16};
    ACTION[24][semi] = {'r', 17};
    ACTION[24][addsub] = {'r', 17};
    ACTION[24][rparen] = {'r', 17};
    ACTION[24][multdiv] = {'r', 17};
    ACTION[25][lbrace] = {'s', 30};
    ACTION[26][rparen] = {'r', 22};
    ACTION[26][comma] = {'s', 32};
    ACTION[27][id] = {'s', 23};
    ACTION[27][num] = {'s', 24};
    ACTION[28][id] = {'s', 23};
    ACTION[28][num] = {'s', 24};
    ACTION[29][addsub] = {'s', 27};
    ACTION[29][rparen] = {'s', 35};
    ACTION[30][vtype] = {'s', 42};
    ACTION[30][id] = {'s', 43};
    ACTION[30][rbrace] = {'r', 24};
    ACTION[30][iff] = {'s', 40};
    ACTION[30][whilee] = {'s', 41};
    ACTION[30][returnn] = {'r', 24};
    ACTION[31][rparen] = {'r', 19};
    ACTION[32][vtype] = {'s', 44};
    ACTION[33][semi] = {'r', 11};
    ACTION[33][addsub] = {'r', 11};
    ACTION[33][rparen] = {'r', 11};
    ACTION[33][multdiv] = {'s', 28};
    ACTION[34][semi] = {'r', 14};
    ACTION[34][addsub] = {'r', 14};
    ACTION[34][rparen] = {'r', 14};
    ACTION[34][multdiv] = {'r', 14};
    ACTION[35][semi] = {'r', 13};
    ACTION[35][addsub] = {'r', 13};
    ACTION[35][rparen] = {'r', 13};
    ACTION[36][returnn] = {'s', 46};
    ACTION[37][vtype] = {'s', 42};
    ACTION[37][id] = {'s', 43};
    ACTION[37][rbrace] = {'r', 24};
    ACTION[37][iff] = {'s', 40};
    ACTION[37][whilee] = {'s', 41};
    ACTION[37][returnn] = {'r', 24};
    ACTION[38][vtype] = {'r', 25};
    ACTION[38][id] = {'r', 25};
    ACTION[38][rbrace] = {'r', 25};
    ACTION[38][iff] = {'r', 25};
    ACTION[38][whilee] = {'r', 25};
    ACTION[38][returnn] = {'r', 25};
    ACTION[39][semi] = {'s', 48};
    ACTION[40][lparen] = {'s', 49};
    ACTION[41][lparen] = {'s', 50};
    ACTION[42][id] = {'s', 51};
    ACTION[43][assign] = {'s', 11};
    ACTION[44][id] = {'s', 52};
    ACTION[45][rbrace] = {'s', 53};
    ACTION[46][id] = {'s', 23};
    ACTION[46][literal] = {'s', 17};
    ACTION[46][character] = {'s', 18};
    ACTION[46][boolstr] = {'s', 19};
    ACTION[46][lparen] = {'s', 21};
    ACTION[46][num] = {'s', 24};
    ACTION[47][rbrace] = {'r', 23};
    ACTION[47][returnn] = {'r', 23};
    ACTION[48][vtype] = {'r', 26};
    ACTION[48][id] = {'r', 26};
    ACTION[48][rbrace] = {'r', 26};
    ACTION[48][iff] = {'r', 26};
    ACTION[48][whilee] = {'r', 26};
    ACTION[48][returnn] = {'r', 26};
    ACTION[49][boolstr] = {'s', 56};
    ACTION[50][boolstr] = {'s', 56};
    ACTION[51][semi] = {'s', 10};
    ACTION[51][assign] = {'s', 11};
    ACTION[52][rparen] = {'r', 22};
    ACTION[52][comma] = {'s', 32};
    ACTION[53][vtype] = {'r', 18};
    ACTION[53][dollar] = {'r', 18};
    ACTION[54][semi] = {'s', 59};
    ACTION[55][rparen] = {'s', 60};
    ACTION[55][comp] = {'s', 61};
    ACTION[56][rparen] = {'r', 30};
    ACTION[56][comp] = {'r', 30};
    ACTION[57][rparen] = {'s', 62};
    ACTION[57][comp] = {'s', 61};
    ACTION[58][rparen] = {'r', 21};
    ACTION[59][rbrace] = {'r', 33};
    ACTION[60][lbrace] = {'s', 63};
    ACTION[61][boolstr] = {'s', 64};
    ACTION[62][lbrace] = {'s', 65};
    ACTION[63][vtype] = {'s', 42};
    ACTION[63][id] = {'s', 43};
    ACTION[63][rbrace] = {'r', 24};
    ACTION[63][iff] = {'s', 40};
    ACTION[63][whilee] = {'s', 41};
    ACTION[63][returnn] = {'r', 24};
    ACTION[64][rparen] = {'r', 29};
    ACTION[64][comp] = {'r', 29};
    ACTION[65][vtype] = {'s', 42};
    ACTION[65][id] = {'s', 43};
    ACTION[65][rbrace] = {'r', 24};
    ACTION[65][iff] = {'s', 40};
    ACTION[65][whilee] = {'s', 41};
    ACTION[65][returnn] = {'r', 24};
    ACTION[66][rbrace] = {'s', 68};
    ACTION[67][rbrace] = {'s', 69};
    ACTION[68][vtype] = {'r', 32};
    ACTION[68][id] = {'r', 32};
    ACTION[68][rbrace] = {'r', 32};
    ACTION[68][iff] = {'r', 32};
    ACTION[68][whilee] = {'r', 32};
    ACTION[68][elsee] = {'s', 71};
    ACTION[68][returnn] = {'r', 32};
    ACTION[69][vtype] = {'r', 28};
    ACTION[69][id] = {'r', 28};
    ACTION[69][rbrace] = {'r', 28};
    ACTION[69][iff] = {'r', 28};
    ACTION[69][whilee] = {'r', 28};
    ACTION[69][returnn] = {'r', 28};
    ACTION[70][vtype] = {'r', 27};
    ACTION[70][id] = {'r', 27};
    ACTION[70][rbrace] = {'r', 27};
    ACTION[70][iff] = {'r', 27};
    ACTION[70][whilee] = {'r', 27};
    ACTION[70][returnn] = {'r', 27};
    ACTION[71][lbrace] = {'s', 72};
    ACTION[72][vtype] = {'s', 42};
    ACTION[72][id] = {'s', 43};
    ACTION[72][rbrace] = {'r', 24};
    ACTION[72][iff] = {'s', 40};
    ACTION[72][whilee] = {'s', 41};
    ACTION[72][returnn] = {'r', 24};
    ACTION[73][rbrace] = {'s', 74};
    ACTION[74][vtype] = {'r', 31};
    ACTION[74][id] = {'r', 31};
    ACTION[74][rbrace] = {'r', 31};
    ACTION[74][iff] = {'r', 31};
    ACTION[74][whilee] = {'r', 31};
    ACTION[74][returnn] = {'r', 31};
}

int find_inttoken(const string s){
    if (s == "vtype") return vtype; 
    else if (s == "num") return num;
    else if (s == "character") return character;
    else if (s == "boolstr") return boolstr;
    else if (s == "literal") return literal;
    else if (s == "id") return id;
    else if (s == "if") return iff;
    else if (s == "else") return elsee;
    else if (s == "while") return whilee;
    else if (s == "return") return returnn;
    else if (s == "addsub") return addsub;
    else if (s == "multdiv") return multdiv;
    else if (s == "assign") return assign;
    else if (s == "comp") return comp;
    else if (s == "semi") return semi;
    else if (s == "comma") return comma;
    else if (s == "lparen") return lparen;
    else if (s == "rparen") return rparen;
    else if (s == "lbrace") return lbrace;
    else if (s == "rbrace") return rbrace;
    else if (s == "dollar") return dollar;
    else return -1;
}

tree* make_child(const int reduction_num, const vector<tree*> &childs){
    string parent_item;
    if(reduction_num == 0) parent_item = "CODE'";
    else if(1 <= reduction_num && reduction_num <= 3) parent_item = "CODE";
    else if(4 <= reduction_num && reduction_num <= 5) parent_item = "VDECL";
    else if(reduction_num == 6) parent_item = "ASSIGN";
    else if(7 <= reduction_num && reduction_num <= 10) parent_item = "RHS";
    else if(11 <= reduction_num && reduction_num <= 13) parent_item = "EXPR";
    else if(14 <= reduction_num && reduction_num <= 15) parent_item = "TERM";
    else if(16 <= reduction_num && reduction_num <= 17) parent_item = "FACTOR";
    else if(reduction_num == 18) parent_item = "FDECL";
    else if(19 <= reduction_num && reduction_num <= 20) parent_item = "ARG";
    else if(21 <= reduction_num && reduction_num <= 22) parent_item = "MOREARGS";
    else if(23 <= reduction_num && reduction_num <= 24) parent_item = "BLOCK";
    else if(25 <= reduction_num && reduction_num <= 28) parent_item = "STMT";
    else if(29 <= reduction_num && reduction_num <= 30) parent_item = "COND";
    else if(31 <= reduction_num && reduction_num <= 32) parent_item = "ELSE";
    else if(reduction_num == 33) parent_item = "RETURN";
    else return NULL;
    tree* parent = new tree(parent_item);
    parent->setchilds(childs);

    return parent;
}

// parse tree representation
//1. 앞선 stickstack 정산   2. 헤드 + 아이템명 출력   3. 자식 각각 printTree 함수 실행시키기        level : 트리의 깊이, stickStack : 각 level 별로 stick이 존재하는지 여부
void printTree(tree root, int level, vector<bool> st, bool isFinalChild) //처음에는 level = 0 stickstack = empty 
{
        int i;
    for (i = 0; i < level - 1; i++)
    {
        if (st[i])
            cout << "│  ";
        else
            cout << "   ";
    }    

    string item = root.getitem();
    if (!isRoot)
    {
        if (isFinalChild)
        {
            cout << "└─" << item;
            st.push_back(false);
        }
        else
        {
            cout << "├─" << item;
            st.push_back(true);
        }       
    }     
    else
    {
        cout << item;
        isRoot = false;
    }
    cout << "\n";
    vector<tree*> childs = root.getchilds();
    int length = childs.size();
    int cnt = 1;
    bool isFinal = false;

    if (childs.empty())
        return;

    for (auto it : childs)
    {
        if (cnt == length)
            isFinal = true;

        printTree(*it, level + 1, st, isFinal);
        cnt++;
    }
}
