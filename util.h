// -*- mode: cpp -*-
#ifndef __NOMLIB_UTIL_H__
#define __NOMLIB_UTIL_H__

#include <iostream>
#include <set>
#define DBGP(MSG) std::cout << __FILE__ << ":" << __LINE__ << ": dbg in " << __FUNCTION__ << "(): " << MSG << std::endl
#define ERRP(MSG) std::cerr << __FILE__ << ":" << __LINE__ << ": error in " << __FUNCTION__ << "(): " << MSG << std::endl

namespace nl{
  // @return container に val が含まれれば true
  template <typename T>
  bool has(const std::set<T> &container, const T &val);
};


#endif
