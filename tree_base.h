// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE_H
#define NL_TREE_INTERFACE_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/01/20 22:17:19
// 木構造を表す interface

#include <list>
#include "ptr_base.h"

namespace nl{

  template <class T>
  class AbsTree : public hasPtr<T>{
  public: // typedef
	typedef typename hasPtr<T>::Ptr Ptr; // 子クラスへのポインタ
	typedef typename hasPtr<T>::WeakPtr WeakPtr; // 子クラスへのポインタ
	typedef std::list< Ptr > List;       // 子クラスのリスト
	
  public: // method
	AbsTree();
	virtual ~AbsTree();
	// 子ノード追加
	void add(const T &node){ add( new T(node) ); }
	void add(      T *node){ add( Ptr( node ) ); }
	void add(Ptr      node){
	  node->this_ptr_ = node;
	  node->parent_   = this_ptr_;
	  node->updateDepth(depth_+1);
	  children_.push_back(node);
	}

  private:
	void updateDepth(unsigned int newDepth){
	  depth_ = newDepth;
	  for( typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
		(*ite)->updateDepth(depth_+1);
	}
	
  protected: // property
	List children_; //std::list< ::Ptr > children;
	WeakPtr parent_;   // 親ノードへのポインタ
	WeakPtr this_ptr_; // 自分自身へのポインタ
	unsigned int depth_; // ルートから数えた深さ。ルートは0
  };
  
};

#endif
