// -*- mode:c++ -*-
// created date : 2011/12/02 00:32:55
// last updated : 2011/12/19 13:49:43

#include "util.h"

#include <cstdio>

namespace nl{
  static char buf_dec2hex[1024];
  // 16進文字列に変換
  const std::string toHex(int dec){
	snprintf(buf_dec2hex, 1023, "%02x", dec);
	return buf_dec2hex;
  }
  const std::string toHex(void *ptr){
	snprintf(buf_dec2hex, 1023, "%08x", (unsigned int)ptr);
	return buf_dec2hex;
  }

  
  // ファイルの長さを返す
  size_t getFileSize(std::istream &ifs){
	std::streampos pos = ifs.tellg();
	size_t fileSize = (size_t)ifs.seekg(0, std::ios::end).tellg();
	ifs.seekg(pos, std::ios::beg);
	return fileSize;
  }

  // ostream <- istream
  void write(std::ostream &out, std::istream &in){
	if((void*)&in == (void*)&out) return;
	int c;
	while( (c=in.get()) != EOF ) out.put(c);
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
