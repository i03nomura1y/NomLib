// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE_H
#define NL_TREE_INTERFACE_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/12/29 20:58:11
// �؍\����\�� interface

#include <list>
#include "variable.h"
#include "util.h"

namespace nl{

  template <class T>
  class AbsTree : public AbsNameTable{
  public: // typedef
	typedef typename Smart<T>::Ptr Ptr; // �q�N���X�ւ̃|�C���^
	//typedef typename AbsNameTable::Ptr PtrNT; // NameTable�p�|�C���^
    typedef typename Smart<AbsNameTable>::WeakPtr WeakPtrNT; // NameTable�p�|�C���^
	typedef std::list< Ptr > List;       // �q�N���X�̃��X�g
	
  public: // method
	AbsTree() : children_(), parent_(), this_ptr_(), depth_(0){}
	virtual ~AbsTree(){}
	void copy( const AbsTree<T> *obj ){
	  children_ = obj->children_;
	  parent_   = obj->parent_;
	  //this_ptr_ 
	  depth_    = obj->depth_;
	}
	/// �q�m�[�h�ǉ�
	void add(const T &node){ add( new T(node) ); }
	void add(      T *node){ add( Ptr( node ) ); }
	void add(Ptr      node){
	  node->this_ptr(node);
	  node->parent_   = this_ptr_;
	  node->updateDepth(depth_+1);
	  children_.push_back(node);
	}
	// key �̌��� node ��ǉ��Bkey ��������Ζ����ɒǉ�
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
	  // key ��������Ȃ����� -> ����
	  children_.push_back(node);
	}
	
	/// �q�m�[�h�폜
	// key ���폜
	bool remove(const T *key);

	/// �q�m�[�h��Ԃ�
	List &children(){ return children_; }
	// idx �Ԗڂ̎q�m�[�h
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
	WeakPtrNT parent_;   // �e�m�[�h�ւ̃|�C���^
	WeakPtrNT this_ptr_; // �������g�ւ̃|�C���^
	unsigned int depth_; // ���[�g���琔�����[���B���[�g��0
  };

  
  
  /// �q�m�[�h�폜
  // key ���폜
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