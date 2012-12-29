// -*- mode:c++ -*-
#ifndef NL_SMART_H
#define NL_SMART_H
// created date : 2012/12/29 19:49:50
// last updated : 2012/12/29 20:57:45
// スマートポインタ関連の typedef

#include <tr1/memory> // shared_ptr, weak_ptr
#include <list>
#include <algorithm>

namespace nl{
    using std::tr1::shared_ptr;
    using std::tr1::weak_ptr;
    
    // T 型のスマートポインタ、リストの typedef
    // 使い方:
    //   Smart<Obj>::Ptr ptr;
    template <class T>
    class Smart{
    public:
        // typedef
        typedef shared_ptr<T> Ptr;
        typedef weak_ptr<T> WeakPtr;
        typedef std::list<Ptr> List;
        //static Ptr nullPtr(){ return Ptr(); }
    };
};

namespace std{
    using nl::Smart;
    
    // list中に obj があれば obj を、なければ NULL( shared_ptr<T>() ) を返す
    template <class T>
    typename Smart<T>::Ptr find( typename Smart<T>::List &list,
                                 const typename Smart<T>::Ptr &obj );

    
    /// 実装 =================================
    template <class T>
    typename Smart<T>::Ptr find( typename Smart<T>::List &list, const typename Smart<T>::Ptr &obj ){
        typename Smart<T>::List::iterator ite = std::find(list.begin(), list.end(), obj);
        if( ite == list.end() ) return Smart<T>::Ptr();
        return *ite;
    }
};


#endif
