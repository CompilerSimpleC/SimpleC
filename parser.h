#include <utility>
#include <vector>
#include <string>
using namespace std;

enum term { vtype, id, semi, assign, literal, character, boolstr, addsub, multdiv, lparen, rparen, num, lbrace, rbrace, comma, iff, whilee, comp, elsee, returnn, dollor };
enum nonterm { CODE, VDECL, ASSIGN, RHS, EXPR, TERM, FACTOR, FDECL, ARG, MOREARGS, BLOCK, STMT, COND, COND_, ELSE, RETURN };

extern pair<char, int> ACTION[74][21];     // row : state, column : terminals
extern int G0T0[74][16];                   // row : state, column : non terminals
extern pair<int, int> reduction[33];       // first : GOTO table의 column값, second : pop할 개수

void init_reduction();      // CFG 각 derivation에 대한 LHS, pop 개수를 지정하는 함수
void InitializeGOTO();      // GOTO table 초기화
void init_ACTION();         // ACTION table 초기화
int find_inttoken(const string s); // token string에 대해 token int로 반환
string make_child(const int reduction_num, const vector<tree*>& childs); // CFG 번호로 부모 tree를 만들어서 자식 vector 할당 후 부모 string을 반환

