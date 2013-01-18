// created date : 2012/01/20 22:15:23
// last updated : 2013/01/18 22:17:39
// 
#include "tree_base.h"

namespace nl{


};

#ifdef TEST
#include <iostream>
using namespace std;

static int cnt = 0;

class Tree : public nl::AbsTree<Tree>{
public:
    int num;
    ~Tree(){}
    void dump(){
        cout << endl;
        for(unsigned int i=0;i<depth_;i++) cout << " ";
        cout << "(" << num;
        for(List::iterator ite = m_list.begin(); ite != m_list.end(); ++ite){
            (*ite)->dump();
        }
        cout << ")";
    }
    static Ptr build(int n){ return Ptr(new Tree(n)); }
private:
    Tree(int n) : num(n){ cnt++; }
    Tree(const Tree &obj) : num(obj.num){ cnt++; }
};

int main(){
    Tree::Ptr tree = Tree::build(0);
    tree->add( Tree::build(10) );
    Tree::Ptr node = Tree::build(0);
    node->add( Tree::build(21) );
    node->add( Tree::build(22) );
    node->add( Tree::build(23) );
    tree->add( node );
    tree->add( Tree::build(30) );
    tree->add( Tree::build(40) );
    tree->dump(); cout << endl;
    return 0;
}
#endif
