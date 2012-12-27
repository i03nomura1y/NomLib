// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE2_H
#define NL_TREE_INTERFACE2_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/12/27 23:15:47
// �؍\����\�� interface

#include <list>
#include <tr1/memory> // shared_ptr, weak_ptr
#include "util.h"

namespace nl{
    using std::tr1::shared_ptr;
    //using std::tr1::weak_ptr;
    
    template <class T>
    class AbsTree2{
    public: // typedef
        typedef shared_ptr<T> Ptr; // �q�N���X�ւ̃|�C���^
        typedef std::list< Ptr > List; // �q�N���X�̃��X�g
    public: // method
        AbsTree2() : children_(), depth_(0){}
        virtual ~AbsTree2(){}
//        void copy( const AbsTree2<T> *obj ){
//            children_ = obj->children_;
//            //parent_   = obj->parent_;
//            //this_ptr_ 
//            depth_    = obj->depth_;
//        }
        /// �q�m�[�h�ǉ�
        void add(const T &node){ Ptr p(new T(node)); add(p); }
        void add(      T *node){ Ptr p(node);        add(p); }
        void add(const Ptr &node){
            //node->this_ptr(node);
            //node->parent_   = this_ptr_;
            node->updateDepth(depth_+1);
            children_.push_back(node);
        }
        // key �̌��� node ��ǉ��Bkey ��������Ζ����ɒǉ�
        void insertAfter(const T *key, Ptr node);
	
        /// �q�m�[�h�폜
        // key ���폜
        bool remove(const T *key);

        /// �q�m�[�h��Ԃ�
        List &children(){ return children_; }
        // idx �Ԗڂ̎q�m�[�h
        Ptr childAt(int idx);
        
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
        List children_; //std::list< T > children;
        //WeakPtrNT parent_;   // �e�m�[�h�ւ̃|�C���^
        //WeakPtrNT this_ptr_; // �������g�ւ̃|�C���^
        unsigned int depth_; // ���[�g���琔�����[���B���[�g��0
    };


    /// ���� -------------------------------------------------------------------------

    // key �̌��� node ��ǉ�
    // key ������ -> �����ɒǉ�
    template <class T>
    void AbsTree2<T>::insertAfter(const T *key, Ptr node){
        //node->this_ptr(node);
        //node->parent_ = this_ptr_;
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
    template <class T>
    bool AbsTree2<T>::remove(const T *key){
        for(typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite){
            if((*ite).get() == key){
                (*ite)->parent_ = Ptr(); // Null
                children_.erase(ite);
                return true;
            }
        }
        return false;
    }
    
    // idx �Ԗڂ̎q�m�[�h
    template <class T>
    typename AbsTree2<T>::Ptr AbsTree2<T>::childAt(int idx){
        if(idx < 0 || idx >= (int)children_.size()) return Ptr();
        for( typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite , --idx)
            if(idx == 0) return *ite;
        return Ptr();
    }

    // depth_ ���X�V
    template <class T>
    void AbsTree2<T>::updateDepth(unsigned int newDepth){
        depth_ = newDepth;
        for( typename List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
            (*ite)->updateDepth(depth_+1);
    }
 };

#endif
