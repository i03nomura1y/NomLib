// -*- mode:c++ -*-
// created date : 2011/12/02 00:32:55
// last updated : 2011/12/18 13:56:49

#include "util.h"

#include <cstdio>

namespace nl{
  static char buf_dec2hex[1024];
  // 16進文字列に変換
  const char *toHex(int dec){
	snprintf(buf_dec2hex, 1023, "%02x", dec);
	return buf_dec2hex;
  }
  const char *toHex(void *ptr){
	snprintf(buf_dec2hex, 1023, "%08x", (unsigned int)ptr);
	return buf_dec2hex;
  }

  // @return container に val が含まれれば true
  template <typename T>
  bool has(const std::set<T> &container, const T &val){
	return (container.find(val)!=container.end());
  }

  // std::string
  template <>
  bool has(const std::set<std::string> &container, const std::string &val){
	return (container.find(val)!=container.end());
  }
  
}; // namespace nl
