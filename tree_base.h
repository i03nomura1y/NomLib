// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE_H
#define NL_TREE_INTERFACE_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/01/21 18:59:44
// 木構造を表す interface

#include <list>
#include "variable.h"
#include "util.h"

namespace nl{

  template <class T>
  class AbsTree : public AbsNameTable , hasPtr<T>{
  public: // typedef
	typedef typename hasPtr<T>::Ptr Ptr; // 子クラスへのポインタ
	//typedef typename AbsNameTable::Ptr PtrNT; // NameTable用ポインタ
	typedef typename AbsNameTable::WeakPtr WeakPtrNT; // NameTable用ポインタ
	typedef std::list< Ptr > List;       // 子クラスのリスト
	
  public: // method
	AbsTree() : children_(), parent_(), this_ptr_(), depth_(0){}
	virtual ~AbsTree(){}
	void copy( const AbsTree<T> *obj ){
	  children_ = obj->children_;
	  parent_   = obj->parent_;
	  //this_ptr_ 
	  depth_    = obj->depth_;
	}
	/// 子ノード追加
	void add(const T &node){ add( new T(node) ); }
	void add(      T *node){ add( Ptr( node ) ); }
	void add(Ptr      node){
	  node->this_ptr(node);
	  node->parent_   = this_ptr_;
	  node->updateDepth(depth_+1);
	  children_.push_back(node);
	}
	// key の後ろに node を追加。key が無ければ末尾に追加
	void insertAfter(const T *key, Ptr node){
	  node->this_ptr(node);
	  node->parent_ = this_ptr_;
	  node->updateDepth(depth_+1);
	  for(typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite){
		if((*ite).get() == key){
		  ++ite;
		  children_.insert(ite, node);
		  return;
		}
	  }
	  // key が見つからなかった -> 末尾
	  children_.push_back(node);
	}
	
	/// 子ノード削除
	// key を削除
	bool remove(const T *key);

	/// 子ノードを返す
	List &children(){ return children_; }
	// idx 番目の子ノード
	Ptr childAt(int idx){
	  if(idx < 0 || idx >= (int)children_.size()) return Ptr();
	  for( typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite , --idx)
		if(idx == 0) return *ite;
	  return Ptr();
	}

	void this_ptr( PtrNT p ){
	  this_ptr_ = p;
	  for( typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite){
		(*ite)->parent_ = this_ptr_;
		(*ite)->this_ptr(*ite);
	  }
	}
	PtrNT this_ptr() const{ return this_ptr_.lock(); }
	
  private:
	void updateDepth(unsigned int newDepth);

  protected: // property
	List children_; //std::list< ::Ptr > children;
	WeakPtrNT parent_;   // 親ノードへのポインタ
	WeakPtrNT this_ptr_; // 自分自身へのポインタ
	unsigned int depth_; // ルートから数えた深さ。ルートは0
  };

  
  
  /// 子ノード削除
  // key を削除
  template <class T>
  bool AbsTree<T>::remove(const T *key){
	for(typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite){
	  if((*ite).get() == key){
		(*ite)->parent_ = Ptr(); // Null
		children_.erase(ite);
		return true;
	  }
	}
	return false;
  }
  
  template <class T>
  void AbsTree<T>::updateDepth(unsigned int newDepth){
	depth_ = newDepth;
	for( typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
	  (*ite)->updateDepth(depth_+1);
  }
  
};

#endif
