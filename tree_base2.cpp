// created date : 2012/01/20 22:15:23
// last updated : 2012/12/27 22:55:39
// 
#include "tree_base2.h"

namespace nl{


};

#ifdef TEST
#include <iostream>
using namespace std;

static int cnt = 0;

class Tree : public nl::AbsTree2<Tree>{
public:
    int num;
    Tree(int n) : num(n){ cnt++; }
    Tree(const Tree &obj) : num(obj.num){ cnt++; }
    ~Tree(){}
    void dump(){
        cout << endl;
        for(unsigned int i=0;i<depth_;i++) cout << " ";
        cout << "(" << num;
        for(List::iterator ite = children_.begin(); ite != children_.end(); ++ite){
            (*ite)->dump();
        }
        cout << ")";
    }
};

int main(){
    Tree::Ptr tree = Tree::Ptr(new Tree(0) );
    tree->add( new Tree(10) );
    Tree::Ptr node = Tree::Ptr(new Tree(20) );
    node->add( new Tree(21) );
    node->add( new Tree(22) );
    node->add( new Tree(23) );
    tree->add( node );
    tree->add( Tree(30) );
    tree->add( Tree(40) );
    tree->dump(); cout << endl;
    return 0;
}
#endif
