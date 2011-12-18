// -*- mode: cpp -*-
// created date : 2011/12/02 00:32:55
// last updated : 2011/12/18 13:55:10
#ifndef __NOMLIB_UTIL_H__
#define __NOMLIB_UTIL_H__

#include <iostream>
#include <set>
#define DBGP(MSG) std::cout << __FILE__ << ":" << __LINE__ << ": dbg in " << __FUNCTION__ << "(): " << MSG << std::endl
#define ERRP(MSG) std::cerr << __FILE__ << ":" << __LINE__ << ": error in " << __FUNCTION__ << "(): " << MSG << std::endl

namespace nl{
  // 16進文字列に変換
  const char *toHex(int dec);
  const char *toHex(void *ptr);

  // @return container に val が含まれれば true
  template <typename T>
  bool has(const std::set<T> &container, const T &val);
  
};


#endif
