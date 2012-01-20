// -*- mode:c++ -*-
#ifndef NL_POINTER_INTERFACE_H
#define NL_POINTER_INTERFACE_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/01/20 20:56:16
// ƒ|ƒCƒ“ƒ^‚ð typedef ‚·‚é interface

#include <tr1/memory> // shared_ptr, weak_ptr

namespace nl{
  using std::tr1::shared_ptr;
  using std::tr1::weak_ptr;

  template <class T>
  class hasPtr{
  public: // typedef
	typedef shared_ptr<T> Ptr;
	typedef weak_ptr  <T> WeakPtr;

  public: // method
	virtual ~hasPtr(){}
	static Ptr nullPtr(){ return Ptr(); }
	
  };
};

#endif
