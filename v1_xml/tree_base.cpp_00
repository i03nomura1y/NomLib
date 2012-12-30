// created date : 2012/01/20 22:15:23
// last updated : 2012/01/20 22:18:25
// 

#include "tree_base.h"
#include "util.h"
namespace nl{
  template<class T>
  AbsTree<T>::AbsTree() : depth_(0){ nl_INC(); }
  template<class T>
  AbsTree<T>::~AbsTree(){ nl_DEC(); }
};

#ifdef TEST
#include <iostream>
using namespace std;


class Tree : public nl::AbsTree<Tree>{
public:
  int num;
  Tree(int n) : num(n){}
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
  tree->dump();
  return 0;
}
#endif
