// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE2_H
#define NL_TREE_INTERFACE2_H
// created date : 2011/12/18 22:43:33
// last updated : 2013/01/18 22:17:31
// �؍\����\�� interface

#include <tr1/memory> // shared_ptr, weak_ptr
#include "smart.h"
#include "util.h"

namespace nl{
    
    template <class T>
    class AbsTree{
    public: // typedef
        typedef typename Smart<T>::Ptr  Ptr;  // �q�N���X�ւ̃|�C���^
        typedef typename Smart<T>::List List; // �q�N���X�̃��X�g
    public: // method
        static Ptr build(){
            Ptr ptr = Ptr(new T());
            ptr->this_ptr_ = ptr;
            return ptr;
        }
        virtual ~AbsTree(){}
        /// �q�m�[�h�ǉ�
        void add(Ptr &node){
            if( !node ) return;
            m_list.push_back(node);
            node->updateDepth(depth_+1);
            node->parent_   = this_ptr_;
        }
	
        /*
        PtrNT this_ptr() const{ return this_ptr_.lock(); }
        //*/
    protected:
        AbsTree() : depth_(0){}
    private:
        void updateDepth(unsigned int newDepth);

    protected: // property
        unsigned int depth_;         // ���[�g���琔�����[���B���[�g��0
        typename Smart<T>::WeakPtr this_ptr_; // �������g�ւ̃|�C���^
        typename Smart<T>::WeakPtr parent_;   // �e�m�[�h�ւ̃|�C���^
        List m_list;
    };


    /// ���� -------------------------------------------------------------------------

    // depth_ ���X�V
    template <class T>
    void AbsTree<T>::updateDepth(unsigned int newDepth){
        depth_ = newDepth;
        for( typename List::iterator ite = m_list.begin(); ite!=m_list.end(); ++ite)
            (*ite)->updateDepth(depth_+1);
    }
};

#endif
