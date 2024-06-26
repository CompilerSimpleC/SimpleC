#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <iostream>
#include <vector>
#include <string>

enum term{vtype, id, semi, assign, literal, character, boolstr, addsub, multdiv, lparen, rparen, num, lbrace, rbrace, comma, iff, whilee, comp, elsee, returnn, dollor};
enum nonterm{CODE, VDECL, ASSIGN, RHS, EXPR, TERM, FACTOR, FDECL, ARG, MOREARGS, BLOCK, STMT, COND, COND_, ELSE, RETURN};
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
pair<char, int> ACTION[74][21];     // row : state       column : terminals
int G0T0[74][16];                   // row : state       column : non terminals
pair<int, int> reduction[33];       // first : GOTO table의 column값.     second : pop할 개수

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
    tokens.push_back("dollor");
    
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
            token_stack.push(t);           // 읽은 token queue에 push(나중에 자식으로 만들기 위함)
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
            token_stack.push(parent_tree); // queue에 부모 string push
            int push_value = G0T0[state_stack.top()][reduction[table_value.second].first]; // stack에 남은 state의 top과 derivation의 LHS의 GOTO table value
            state_stack.push(push_value);       // GOTO table value를 state stack에 push
        }
        else if(table_value.first == 'a'){  // accept
            //자식 vector 설정, token queue에 있는 모든 string을 자식으로 만듦
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

            if(child_vector.size() == 0){
                tree* epsilon_tree = new tree("epsilon");
                child_vector.push_back(epsilon_tree);
            }

            // 부모 tree 만들고 자식vector 할당
            root_tree = new tree("CODE");
            root_tree->setchilds(child_vector);
            break;
        }
        else {
            // 에러처리. 파싱에러
            cout << "Error: Token" << input_data <<  "not matched ACTION table at [" << state_stack.top() << ", " << input_data << "]" << endl;
            return 0;
        }
    }
    
    // representation of parse tree
    vector<bool> stickStateStack;
    printTree(*root_tree, 0, stickStateStack, true);
    return 0;
}

void init_reduction(){
    reduction[0].first = CODE; reduction[0].second = 2;         // CODE -> VDECL CODE
    reduction[1].first = CODE; reduction[1].second = 2;         // CODE -> FDECL CODE 
    reduction[2].first = CODE; reduction[2].second = 0;         // CODE -> epsilon
    reduction[3].first = VDECL; reduction[3].second = 3;        // VDECL -> vtype id semi
    reduction[4].first = VDECL; reduction[4].second = 3;        // VDECL -> vtype ASSIGN semi
    reduction[5].first = ASSIGN; reduction[5].second = 3;       // ASSIGN -> id assign RHS
    reduction[6].first = RHS; reduction[6].second = 1;          // RHS -> EXPR
    reduction[7].first = RHS; reduction[7].second = 1;          // RHS -> literal
    reduction[8].first = RHS; reduction[8].second = 1;          // RHS -> character
    reduction[9].first = RHS; reduction[9].second = 1;          // RHS -> boolstr
    reduction[10].first = EXPR; reduction[10].second = 3;       // EXPR -> EXPR addsub TERM
    reduction[11].first = EXPR; reduction[11].second = 1;       // EXPR -> TERM
    reduction[12].first = EXPR; reduction[12].second = 3;       // EXPR -> lparen EXPR rparen
    reduction[13].first = TERM; reduction[13].second = 3;       // TERM -> TERM multdiv FACTOR
    reduction[14].first = TERM; reduction[14].second = 1;       // TERM -> FACTOR
    reduction[15].first = FACTOR; reduction[15].second = 1;     // FACTOR -> id
    reduction[16].first = FACTOR; reduction[16].second = 1;     // FACTOR -> num
    reduction[17].first = FDECL; reduction[17].second = 9;      // FDECL -> vtype id lparen ARG rparen lbrace BLOCK RETURN rbrace
    reduction[18].first = ARG; reduction[18].second = 3;        // ARG -> vtype id MOREARGS
    reduction[19].first = ARG; reduction[19].second = 0;        // ARG -> epsilon
    reduction[20].first = MOREARGS; reduction[20].second = 4;   // MOREARGS -> comma vtype id MOREARGS
    reduction[21].first = MOREARGS; reduction[21].second = 0;   // MOREARGS -> epsilon
    reduction[22].first = BLOCK; reduction[22].second = 2;      // BLOCK -> STMT BLOCK
    reduction[23].first = BLOCK; reduction[23].second = 0;      // BLOCK -> epsilon
    reduction[24].first = STMT; reduction[24].second = 1;       // STMT -> VDECL
    reduction[25].first = STMT; reduction[25].second = 2;       // STMT -> ASSIGN semi
    reduction[26].first = STMT; reduction[26].second = 8;       // STMT -> if lparen COND rparen lbrace BLOCK rbrace ELSE
    reduction[27].first = STMT; reduction[27].second = 7;       // STMT -> while lparen COND rparen lbrace BLOCK rbrace
    reduction[28].first = COND; reduction[28].second = 3;       // COND -> COND comp boolstr
    reduction[29].first = COND; reduction[29].second = 1;       // COND -> boolstr
    reduction[30].first = ELSE; reduction[30].second = 4;       // ELSE -> else lbrace BLOCK rbrace
    reduction[31].first = ELSE; reduction[31].second = 0;       // ELSE -> epsilon
    reduction[32].first = RETURN; reduction[32].second = 3;     // RETURN -> return RHS semi
}

void InitializeGOTO(){
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

void init_ACTION(){
    // vtype
    ACTION[0][vtype] = {'s', 2}; ACTION[1][vtype] = {'s', 5}; ACTION[4][vtype] = {'s', 5}; ACTION[10][vtype] = {'r', 3}; ACTION[12][vtype] = {'r', 4}; ACTION[13][vtype] = {'s', 25}; ACTION[34][vtype] = {'s', 2}; ACTION[36][vtype] = {'s', 44}; ACTION[38][vtype] = {'s', 2}; ACTION[39][vtype] = {'r', 24}; ACTION[48][vtype] = {'r', 25}; ACTION[52][vtype] = {'r', 17}; ACTION[62][vtype] = {'s', 2}; ACTION[64][vtype] = {'s', 2}; ACTION[67][vtype] = {'r', 31}; ACTION[68][vtype] = {'r', 27}; ACTION[69][vtype] = {'r', 26}; ACTION[71][vtype] = {'s', 2}; ACTION[73][vtype] = {'r', 30};
    // id
    ACTION[2][id] = {'s', 6}; ACTION[5][id] = {'s', 9}; ACTION[10][id] = {'r', 3}; ACTION[11][id] = {'s', 22}; ACTION[12][id] = {'r', 4}; ACTION[20][id] = {'s', 22}; ACTION[25][id] = {'s', 30}; ACTION[26][id] = {'s', 22}; ACTION[27][id] = {'s', 22}; ACTION[34][id] = {'s', 43}; ACTION[38][id] = {'s', 43}; ACTION[39][id] = {'r', 24}; ACTION[44][id] = {'s', 51}; ACTION[46][id] = {'s', 22}; ACTION[48][id] = {'r', 25}; ACTION[62][id] = {'s', 43}; ACTION[64][id] = {'s', 43}; ACTION[67][id] = {'r', 31}; ACTION[68][id] = {'r', 27}; ACTION[69][id] = {'r', 26}; ACTION[71][id] = {'s', 43}; ACTION[73][id] = {'r', 30}; 
    // semi
    ACTION[6][semi] = {'s', 10}; ACTION[7][semi] = {'s', 12}; ACTION[9][semi] = {'s', 10}; ACTION[14][semi] = {'r', 5}; ACTION[15][semi] = {'r', 6}; ACTION[16][semi] = {'r', 7}; ACTION[17][semi] = {'r', 8}; ACTION[18][semi] = {'r', 9}; ACTION[19][semi] = {'r', 11}; ACTION[21][semi] = {'r', 14}; ACTION[22][semi] = {'r', 15}; ACTION[23][semi] = {'r', 16}; ACTION[31][semi] = {'r', 10}; ACTION[32][semi] = {'r', 13}; ACTION[33][semi] = {'r', 12}; ACTION[40][semi] = {'s', 48}; ACTION[53][semi] = {'s', 58};
    // assign
    ACTION[6][assign] = {'s', 11}; ACTION[9][assign] = {'s', 11}; ACTION[43][assign] = {'s', 11};
    // literal
    ACTION[11][literal] = {'s', 16}; ACTION[46][literal] = {'s', 16};
    // character
    ACTION[11][character] = {'s', 17}; ACTION[46][character] = {'s', 17};
    // boolstr
    ACTION[11][boolstr] = {'s', 18}; ACTION[46][boolstr] = {'s', 18}; ACTION[49][boolstr] = {'s', 55}; ACTION[50][boolstr] = {'s', 55}; ACTION[60][boolstr] = {'s', 63};
    // addsub
    ACTION[15][addsub] = {'s', 26}; ACTION[19][addsub] = {'r', 11}; ACTION[21][addsub] = {'r', 14}; ACTION[22][addsub] = {'r', 15}; ACTION[23][addsub] = {'r', 16}; ACTION[28][addsub] = {'s', 26}; ACTION[31][addsub] = {'r', 10}; ACTION[32][addsub] = {'r', 13}; ACTION[33][addsub] = {'r', 12};
    // lparen
    ACTION[9][lparen] = {'s', 13}; ACTION[11][lparen] = {'s', 20}; ACTION[20][lparen] = {'s', 20}; ACTION[41][lparen] = {'s', 49}; ACTION[42][lparen] = {'s', 50}; ACTION[46][lparen] = {'s', 20};
    // rparen
    ACTION[13][rparen] = {'r', 19}; ACTION[19][rparen] = {'r', 11}; ACTION[21][rparen] = {'r', 14}; ACTION[22][rparen] = {'r', 15}; ACTION[23][rparen] = {'r', 16}; ACTION[24][rparen] = {'s', 29}; ACTION[28][rparen] = {'s', 33}; ACTION[30][rparen] = {'r', 21}; ACTION[31][rparen] = {'r', 10}; ACTION[32][rparen] = {'r', 13}; ACTION[33][rparen] = {'r', 12}; ACTION[35][rparen] = {'r', 18}; ACTION[51][rparen] = {'r', 21}; ACTION[54][rparen] = {'s', 59}; ACTION[55][rparen] = {'r', 29}; ACTION[56][rparen] = {'s', 61}; ACTION[57][rparen] = {'r', 20}; ACTION[63][rparen] = {'r', 28};
    // multdiv
    ACTION[19][multdiv] = {'s', 27}; ACTION[21][multdiv] = {'r', 14}; ACTION[22][multdiv] = {'r', 15}; ACTION[23][multdiv] = {'r', 16}; ACTION[31][multdiv] = {'s', 27}; ACTION[32][multdiv] = {'r', 13};
    // num
    ACTION[11][num] = {'s', 23}; ACTION[20][num] = {'s', 23}; ACTION[26][num] = {'s', 23}; ACTION[27][num] = {'s', 23}; ACTION[46][num] = {'s', 23};
    // lbrace
    ACTION[29][lbrace] = {'s', 34}; ACTION[59][lbrace] = {'s', 62}; ACTION[61][lbrace] = {'s', 64}; ACTION[70][lbrace] = {'s', 71};
    // rbrace
    ACTION[10][rbrace] = {'r', 3}; ACTION[12][rbrace] = {'r', 4}; ACTION[34][rbrace] = {'r', 23}; ACTION[38][rbrace] = {'r', 23}; ACTION[39][rbrace] = {'r', 24}; ACTION[45][rbrace] = {'s', 52}; ACTION[47][rbrace] = {'r', 22}; ACTION[48][rbrace] = {'r', 25}; ACTION[58][rbrace] = {'r', 32}; ACTION[62][rbrace] = {'r', 23}; ACTION[64][rbrace] = {'r', 23}; ACTION[65][rbrace] = {'s', 67}; ACTION[66][rbrace] = {'s', 68}; ACTION[67][rbrace] = {'r', 31}; ACTION[68][rbrace] = {'r', 27}; ACTION[69][rbrace] = {'r', 26}; ACTION[71][rbrace] = {'r', 23}; ACTION[72][rbrace] = {'s', 73}; ACTION[73][rbrace] = {'r', 30}; 
    // comma
    ACTION[30][comma] = {'s', 36}; ACTION[51][comma] = {'s', 36};
    // iff
    ACTION[10][iff] = {'r', 3}; ACTION[12][iff] = {'r', 4}; ACTION[34][iff] = {'s', 41}; ACTION[38][iff] = {'s', 41}; ACTION[39][iff] = {'r', 24}; ACTION[48][iff] = {'r', 25}; ACTION[62][iff] = {'s', 41}; ACTION[64][iff] = {'s', 41}; ACTION[67][iff] = {'r', 31}; ACTION[68][iff] = {'r', 27}; ACTION[69][iff] = {'r', 26}; ACTION[71][iff] = {'s', 41}; ACTION[73][iff] = {'r', 30};
    // whilee
    ACTION[10][whilee] = {'r', 3}; ACTION[12][whilee] = {'r', 4}; ACTION[34][whilee] = {'s', 42}; ACTION[38][whilee] = {'s', 42}; ACTION[39][whilee] = {'r', 24}; ACTION[48][whilee] = {'r', 25}; ACTION[62][whilee] = {'s', 42}; ACTION[64][whilee] = {'s', 42}; ACTION[67][whilee] = {'r', 31}; ACTION[68][whilee] = {'r', 27}; ACTION[69][whilee] = {'r', 26}; ACTION[71][whilee] = {'s', 42}; ACTION[73][whilee] = {'r', 30};
    // comp
    ACTION[54][comp] = {'s', 60}; ACTION[55][comp] = {'r', 29}; ACTION[56][comp] = {'s', 60}; ACTION[63][comp] = {'r', 28};
    // elsee
    ACTION[67][elsee] = {'s', 70};
    // returnn
    ACTION[10][returnn] = {'r', 3}; ACTION[12][returnn] = {'r', 4}; ACTION[34][returnn] = {'r', 23}; ACTION[37][returnn] = {'s', 46}; ACTION[38][returnn] = {'r', 23}; ACTION[39][returnn] = {'r', 24}; ACTION[47][returnn] = {'r', 22}; ACTION[48][returnn] = {'r', 25}; ACTION[62][returnn] = {'r', 23}; ACTION[64][returnn] = {'r', 23}; ACTION[67][returnn] = {'r', 31}; ACTION[68][returnn] = {'r', 27}; ACTION[69][returnn] = {'r', 26}; ACTION[71][returnn] = {'r', 23}; ACTION[73][returnn] = {'r', 30};
    // dollor
    ACTION[1][dollor] = {'r', 2}; ACTION[3][dollor] = {'a', 0}; ACTION[4][dollor] = {'r', 2}; ACTION[8][dollor] = {'r', 1}; ACTION[10][dollor] = {'r', 3}; ACTION[12][dollor] = {'r', 4}; ACTION[52][dollor] = {'r', 17};
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
    else if (s == "dollor") return dollor;
    else return -1;
}

tree* make_child(const int reduction_num, const vector<tree*> &childs){
    string parent_item;
    if(0 <= reduction_num && reduction_num <= 2) parent_item = "CODE";
    else if(3 <= reduction_num && reduction_num <= 4) parent_item = "VDECL";
    else if(reduction_num == 5) parent_item = "ASSIGN";
    else if(6 <= reduction_num && reduction_num <= 9) parent_item = "RHS";
    else if(10 <= reduction_num && reduction_num <= 12) parent_item = "EXPR";
    else if(13 <= reduction_num && reduction_num <= 14) parent_item = "TERM";
    else if(15 <= reduction_num && reduction_num <= 16) parent_item = "FACTOR";
    else if(reduction_num == 17) parent_item = "FDECL";
    else if(18 <= reduction_num && reduction_num <= 19) parent_item = "ARG";
    else if(20 <= reduction_num && reduction_num <= 21) parent_item = "MOREARGS";
    else if(22 <= reduction_num && reduction_num <= 23) parent_item = "BLOCK";
    else if(24 <= reduction_num && reduction_num <= 27) parent_item = "STMT";
    else if(28 <= reduction_num && reduction_num <= 29) parent_item = "COND";
    else if(30 <= reduction_num && reduction_num <= 31) parent_item = "ELSE";
    else if(reduction_num == 32) parent_item = "RETURN";
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
