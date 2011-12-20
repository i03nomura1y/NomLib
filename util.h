// -*- mode: cpp -*-
// created date : 2011/12/02 00:32:55
// last updated : 2011/12/19 13:46:28
#ifndef __NOMLIB_UTIL_H__
#define __NOMLIB_UTIL_H__

#include <iostream>
#include <set>

#define DBGP(MSG) std::cout << __FILE__ << ":" << __LINE__ << ": dbg in " << __FUNCTION__ << "(): " << MSG << std::endl
#define ERRP(MSG) std::cerr << __FILE__ << ":" << __LINE__ << ": error in " << __FUNCTION__ << "(): " << MSG << std::endl

namespace nl{
  // 16進文字列に変換
  const std::string toHex(int dec);
  const std::string toHex(void *ptr);
  
  /// ファイル関連
  // ファイルの長さを返す
  size_t getFileSize(std::istream &is);
  // ostream <- istream
  void write(std::ostream &out, std::istream &in);
  
  /// コンテナ関連
  // @return container に val が含まれれば true
  template <typename T>
  bool has(const std::set<T> &container, const T &val);
  
};


#endif
