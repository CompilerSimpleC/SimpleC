#include <iostream>
#include <vector>
#include <string>
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
    bool getisLeaf() {
        return this->isLeaf;
    }

    void setitem(string s) {
        this->item = s;
    }
    void setchilds(vector<tree*> c) {
        this->childs = c;
    }
    void setisLeaf(bool b) {
        this->isLeaf = b;
    }
private:
    string item;
    vector<tree*> childs;
    bool isLeaf;
};

bool isRoot = true;
//1. 앞선 stickstack 정산   2. 헤드 + 아이템명 출력   3. 자식 각각 printTree 함수 실행시키기        level : 트리의 깊이, stickStack : 각 level 별로 stick이 존재하는지 여부
void printTree(tree root, int level, vector<bool> st, bool isFinalChild) //처음에는 level = 0 stickstack = empty 
{
    int i;
    for (i = 0; i < level - 1; i++) {
        if (st[i])
            cout << "¿  ";
        else
            cout << "   ";
    }

    string item = root.getitem();
    if (!isRoot) {
        if (isFinalChild) {
            cout << "¿¿" << item;
            st.push_back(false);
        } else {
            cout << "¿¿" << item;
            st.push_back(true);
        }
    } else {
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
