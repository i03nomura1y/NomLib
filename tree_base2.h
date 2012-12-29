// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE2_H
#define NL_TREE_INTERFACE2_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/12/29 21:13:44
// �؍\����\�� interface

#include <tr1/memory> // shared_ptr, weak_ptr
#include "smart.h"
#include "util.h"

namespace nl{
    using std::tr1::shared_ptr;
    //using std::tr1::weak_ptr;
    
    template <class T>
    class AbsTree2{
    public: // typedef
        typedef typename Smart<T>::Ptr  Ptr;  // �q�N���X�ւ̃|�C���^
        typedef typename Smart<T>::List List; // �q�N���X�̃��X�g
    public: // method
        AbsTree2() : depth_(0){}
        virtual ~AbsTree2(){}
        /// �q�m�[�h�ǉ�
        void add(const Ptr &node){
            //node->this_ptr(node);
            //node->parent_   = this_ptr_;
            node->updateDepth(depth_+1);
            m_list.push_back(node);
        }
	
        /// �q�m�[�h��Ԃ�
        List &children(){ return m_list; }
        // idx �Ԗڂ̎q�m�[�h
        //Ptr childAt(int idx){ return SmartList<T>::objAt(idx); }
        
        /*
        void this_ptr( PtrNT p ){
            this_ptr_ = p;
            for( typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite){
                (*ite)->parent_ = this_ptr_;
                (*ite)->this_ptr(*ite);
            }
        }
        PtrNT this_ptr() const{ return this_ptr_.lock(); }
        //*/
    private:
        void updateDepth(unsigned int newDepth);

    protected: // property
        //WeakPtrNT parent_;   // �e�m�[�h�ւ̃|�C���^
        //WeakPtrNT this_ptr_; // �������g�ւ̃|�C���^
        unsigned int depth_; // ���[�g���琔�����[���B���[�g��0
        List m_list;
    };


    /// ���� -------------------------------------------------------------------------

    // depth_ ���X�V
    template <class T>
    void AbsTree2<T>::updateDepth(unsigned int newDepth){
        depth_ = newDepth;
        for( typename List::iterator ite = m_list.begin(); ite!=m_list.end(); ++ite)
            (*ite)->updateDepth(depth_+1);
    }
};

#endif
