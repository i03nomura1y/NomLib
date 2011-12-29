// -*- mode: cpp -*-
// created date : 2011/12/02 00:32:55
// last updated : 2011/12/30 00:01:49
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
  
  /// String
  // 文字列 text が suffix で終わっていれば true
  bool strEndsWith(const std::string &text, const std::string &suffix);

  /// File
  // ファイルの長さを返す
  size_t getFileSize(std::istream &is);
  // 親ディレクトリのパスを返す
  std::string getParentPath(const std::string &path);

  /// Stream
  // ostream <- istream
  void write(std::ostream &out, std::istream &in);
  
  /// Container
  // @return container に val が含まれれば true
  template <typename T>
  bool has(const std::set<T> &container, const T &val);
  
};


#endif
