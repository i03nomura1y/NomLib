// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE2_H
#define NL_TREE_INTERFACE2_H
// created date : 2011/12/18 22:43:33
// last updated : 2013/01/18 22:17:31
// 木構造を表す interface

#include <tr1/memory> // shared_ptr, weak_ptr
#include "smart.h"
#include "util.h"

namespace nl{
    
    template <class T>
    class AbsTree{
    public: // typedef
        typedef typename Smart<T>::Ptr  Ptr;  // 子クラスへのポインタ
        typedef typename Smart<T>::List List; // 子クラスのリスト
    public: // method
        static Ptr build(){
            Ptr ptr = Ptr(new T());
            ptr->this_ptr_ = ptr;
            return ptr;
        }
        virtual ~AbsTree(){}
        /// 子ノード追加
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
        unsigned int depth_;         // ルートから数えた深さ。ルートは0
        typename Smart<T>::WeakPtr this_ptr_; // 自分自身へのポインタ
        typename Smart<T>::WeakPtr parent_;   // 親ノードへのポインタ
        List m_list;
    };


    /// 実装 -------------------------------------------------------------------------

    // depth_ を更新
    template <class T>
    void AbsTree<T>::updateDepth(unsigned int newDepth){
        depth_ = newDepth;
        for( typename List::iterator ite = m_list.begin(); ite!=m_list.end(); ++ite)
            (*ite)->updateDepth(depth_+1);
    }
};

#endif
