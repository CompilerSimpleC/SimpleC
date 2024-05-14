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
    string item; // type 아직 미지정
    vector<tree*> childs;
    bool isLeaf;
};

bool isRoot = true;
//1. 앞선 스틱카운트 정산   2. 헤드 + 아이템명 출력   3. 자식 각각 printTree 함수 실행시키기        level : 트리의 깊이, stickStack : 각 level 별로 stick이 존재하는지 여부
void printTree(tree root, int level, vector<bool> st, bool isFinalChild) //처음에는 level = 0 stickcnt=0 
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

int main()
{
    tree root("root");
    tree one("one");
    tree two("two");
    tree three("three");
    tree four("four");
    tree five("five");
    tree six("six");
    tree seven("seven");
    tree eight("eight");
    tree nine("nine");
    tree ten("ten");
    vector<tree*> child;
    child.push_back(&one);
    child.push_back(&two);
    child.push_back(&three);
    root.setchilds(child);
    child.clear();
    child.push_back(&four);
    one.setchilds(child);
    child.clear();
    child.push_back(&five);
    child.push_back(&six);
    two.setchilds(child);
    child.clear();
    child.push_back(&seven);
    child.push_back(&eight);
    three.setchilds(child);
    child.clear();
    child.push_back(&nine);
    child.push_back(&ten);
    five.setchilds(child);

    vector<bool> stickStateStack;
    printTree(root, 0, stickStateStack, true);
}
