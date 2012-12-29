// -*- mode:c++ -*-
#ifndef NL_TREE_INTERFACE2_H
#define NL_TREE_INTERFACE2_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/12/29 21:13:44
// 木構造を表す interface

#include <tr1/memory> // shared_ptr, weak_ptr
#include "smart.h"
#include "util.h"

namespace nl{
    using std::tr1::shared_ptr;
    //using std::tr1::weak_ptr;
    
    template <class T>
    class AbsTree2{
    public: // typedef
        typedef typename Smart<T>::Ptr  Ptr;  // 子クラスへのポインタ
        typedef typename Smart<T>::List List; // 子クラスのリスト
    public: // method
        AbsTree2() : depth_(0){}
        virtual ~AbsTree2(){}
        /// 子ノード追加
        void add(const Ptr &node){
            //node->this_ptr(node);
            //node->parent_   = this_ptr_;
            node->updateDepth(depth_+1);
            m_list.push_back(node);
        }
	
        /// 子ノードを返す
        List &children(){ return m_list; }
        // idx 番目の子ノード
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
        //WeakPtrNT parent_;   // 親ノードへのポインタ
        //WeakPtrNT this_ptr_; // 自分自身へのポインタ
        unsigned int depth_; // ルートから数えた深さ。ルートは0
        List m_list;
    };


    /// 実装 -------------------------------------------------------------------------

    // depth_ を更新
    template <class T>
    void AbsTree2<T>::updateDepth(unsigned int newDepth){
        depth_ = newDepth;
        for( typename List::iterator ite = m_list.begin(); ite!=m_list.end(); ++ite)
            (*ite)->updateDepth(depth_+1);
    }
};

#endif
