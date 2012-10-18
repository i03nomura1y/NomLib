// -*- mode: cpp -*-
// created date : 2011/12/02 00:32:55
// last updated : 2012/10/18 19:45:27
#ifndef __NOMLIB_UTIL_H__
#define __NOMLIB_UTIL_H__

#include <iostream>
#include <set>
#include <list>
#include <typeinfo> // for typeid()

#define MSGP(MSG) std::cout << MSG << "    (msg from "__FILE__ << ":" << __LINE__ << ")" << std::endl
#define DBGP(MSG) std::cout << __FILE__ << ":" << __LINE__ << ": dbg in " << __FUNCTION__ << "(): " << MSG << std::endl
#define DBGNL()   std::cout << std::endl
#define ERRP(MSG) std::cerr << __FILE__ << ":" << __LINE__ << ": error in " << __FUNCTION__ << "(): " << MSG << std::endl

// new/delete のチェック用
#define nl_INC() nl::inc( typeid(*this).name() )
#define nl_DEC() nl::dec( typeid(*this).name() )

namespace nl{
  // 16進文字列に変換
  const std::string toHex(int dec);
  const std::string toHex(void *ptr);
  
  /// new/delete のチェック用カウンタ
  void inc(const std::string &key);
  void dec(const std::string &key);
  void dump_alloc_status();
  
  /// String
  // 文字列 text が suffix で終わっていれば true
  bool strEndsWith(const std::string &text, const std::string &suffix);

  /// File
  // ファイルの長さを返す
  size_t getFileSize(std::istream &is);
  // 拡張子を返す。無ければ ""
  std::string getExtension(const std::string &path);
  // 親ディレクトリのパスを返す
  std::string getParentPath(const std::string &path);
  /**
   * 拡張子 ext の mimetype を返す。
   * ex. 'html' -> 'text/html'
   */
  std::string ext2mimetype(const std::string &ext);

  /// Stream
  // ostream <- istream
  void write(std::ostream &out, std::istream &in);
  // ostream <- istream
  // 書き込みバイト数を指定
  void write(std::ostream &out, std::istream &in, unsigned int byte_count);
  // out に num を little-endian で put
  // @param byte_count  :  int は何バイト分? (通常は 4)
  void writeAsLittleEndian(std::ostream &out, int num, unsigned int byte_count);
  // in から little-endian で get
  // @param byte_count  :  int は何バイト分? (通常は 4)
  unsigned int readAsLittleEndian(std::istream &in, unsigned int byte_count);
  
  /// Container
  // @return container に val が含まれれば true
  template <typename T>
  bool has(const std::set<T> &container, const T &val);
  
};


#endif
