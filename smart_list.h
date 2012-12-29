// -*- mode:c++ -*-
#ifndef NL_SMART_LIST_H
#define NL_SMART_LIST_H
// created date : 2012/12/29 17:49:50
// last updated : 2012/12/29 17:52:11
// スマートポインタのリスト

#include <list>
#include <tr1/memory> // shared_ptr, weak_ptr

namespace nl{
    using std::tr1::shared_ptr;
    //using std::tr1::weak_ptr;

    template <class T>
    class SmartList{
    public: // typedef
        typedef shared_ptr<T> Ptr;   // 要素へのポインタ
        typedef std::list<Ptr> List; // リスト
        
    protected: // property
        List m_list; // リスト実体
        
    public: // method
        SmartList() : m_list(){}
        virtual ~SmartList(){}
        /// 子要素追加
        void add(const Ptr &obj){ m_list.push_back(obj); }
        // key を削除。削除できたら true
        bool remove(const T *key);
        // idx 番目の要素
        Ptr objAt(int idx);
        
        // // key の後ろに obj を追加。key が無ければ末尾に追加
        // void insertAfter(const T *key, Ptr obj);
    };

    /// 実装 -------------------------------------------------------------------------

    // key を削除。削除できたら true
    template <class T>
    bool SmartList<T>::remove(const T *key){
        for(typename List::iterator ite = m_list.begin(); ite!=m_list.end(); ++ite){
            if((*ite).get() == key){
                m_list.erase(ite);
                return true;
            }
        }
        return false;
    }
    
    // idx 番目の要素
    template <class T>
    typename SmartList<T>::Ptr SmartList<T>::objAt(int idx){
        if(idx < 0 || idx >= (int)m_list.size()) return Ptr();
        for( typename List::iterator ite = m_list.begin(); ite!=m_list.end(); ++ite , --idx)
            if(idx == 0) return *ite;
        return Ptr();
    }

    // // key の後ろに node を追加 key が無い -> 末尾に追加
    // template <class T>
    // void SmartList<T>::insertAfter(const T *key, Ptr obj){
    //     for(typename List::iterator ite = m_list.begin(); ite!=m_list.end(); ++ite){
    //         if( (*ite).get() == key ){
    //             ++ite;
    //             m_list.insert(ite, obj);
    //             return;
    //         }
    //     }
    //     // key が見つからなかった -> 末尾
    //     m_list.push_back(obj);
    // }
    
};



#endif
