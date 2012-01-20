// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE_H
#define NL_TREE_INTERFACE_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/01/20 22:17:19
// �؍\����\�� interface

#include <list>
#include "ptr_base.h"

namespace nl{

  template <class T>
  class AbsTree : public hasPtr<T>{
  public: // typedef
	typedef typename hasPtr<T>::Ptr Ptr; // �q�N���X�ւ̃|�C���^
	typedef typename hasPtr<T>::WeakPtr WeakPtr; // �q�N���X�ւ̃|�C���^
	typedef std::list< Ptr > List;       // �q�N���X�̃��X�g
	
  public: // method
	AbsTree();
	virtual ~AbsTree();
	// �q�m�[�h�ǉ�
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
	WeakPtr parent_;   // �e�m�[�h�ւ̃|�C���^
	WeakPtr this_ptr_; // �������g�ւ̃|�C���^
	unsigned int depth_; // ���[�g���琔�����[���B���[�g��0
  };
  
};

#endif
