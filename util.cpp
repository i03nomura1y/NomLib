// -*- mode:c++ -*-
// created date : 2011/12/02 00:32:55
// last updated : 2012/11/27 16:55:54

#include "util.h"

#include <cstdio>

#include <cxxabi.h> // for demangle
#include <map>

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

  /// new/delete のチェック用カウンタ
  //static unsigned int alloc_cnt = 0;
  static std::map<std::string, int> alloc_cnt;
  void inc(const std::string &key){
	//DBGP(key);
	std::map<std::string, int>::iterator ite = alloc_cnt.find(key);
	if( ite == alloc_cnt.end() ) alloc_cnt.insert(std::pair<std::string, int>(key,1));
	else ite->second++;
  }
  void dec(const std::string &key){
	//DBGP(key);
	alloc_cnt.find(key)->second--;
  }
  void dump_alloc_status(){
	DBGP("");
	for( std::map<std::string, int>::iterator ite = alloc_cnt.begin();
		 ite!=alloc_cnt.end(); ++ite)
	  if(ite->second != 0) std::cout << " " <<  abi::__cxa_demangle(ite->first.c_str(),0,0,NULL) << " : " << ite->second << std::endl;
  }

  // 文字列 text が suffix で終わっていれば true
  bool strEndsWith(const std::string &text, const std::string &suffix){
	unsigned int len = suffix.size();
	return ( text.size() >= len && text.substr( text.size()-len) == suffix );
  }

  // ファイルの長さを返す
  size_t getFileSize(std::istream &ifs){
	std::streampos pos = ifs.tellg();
	size_t fileSize = (size_t)ifs.seekg(0, std::ios::end).tellg();
	ifs.seekg(pos, std::ios::beg);
	return fileSize;
  }
  // 拡張子を返す。無ければ ""
  std::string getExtension(const std::string &path){
    int pos = path.find_last_of('.');
    if(pos > 0) return path.substr( pos+1 );
    return "";
  }
  // 親ディレクトリのパスを返す
  std::string getParentPath(const std::string &path){
	int pos = path.find_last_of('/');
	if(pos > 0) return path.substr( 0, pos );
	if(pos == 0) return "/";
	return "";
  }
  /**
   * 拡張子 ext の mimetype を返す。
   * ex. 'html' -> 'text/html'
   */
  std::string ext2mimetype(const std::string &ext){
    if( ext == "txt" ) return "text/plain";

    if( ext == "html" || ext == "htm" || ext == "" ) return "text/html";
    if( ext == "js"  ) return "text/javascript";
    if( ext == "css" ) return "text/css";
  
    if( ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if( ext == "png" ) return "image/png";
    if( ext == "gif" ) return "image/gif";
    if( ext == "ico" ) return "image/x-icon";

    if( ext == "m4v" ) return "video/x-m4v";
    if( ext == "mp4" ) return "video/mp4";
    if( ext == "mpg" ) return "video/mpeg";
    if( ext == "3gp" ) return "video/3gpp"; // audio/3gpp も有り得る。

    // for HTTP Live Streaming
    if( ext == "ts" ) return "video/MP2T";
    if( ext == "m3u8" ) return "application/x-mpegURL";
  
    DBGP("unknown extension '" << ext << "'");
    return "";
  }

  // ostream <- istream
  void write(std::ostream &out, std::istream &in){
	if((void*)&in == (void*)&out) return;
	int c;
	while( (c=in.get()) != EOF ) out.put(c);
  }
  // ostream <- istream
  // 書き込みバイト数を指定
  void write(std::ostream &out, std::istream &in, unsigned int byte_count){
    if((void*)&in == (void*)&out) return;
	int c;
	for(unsigned int i=0; i<byte_count && (c=in.get()) != EOF; i++ )
      out.put(c);
  }
  // out に num を little-endian で put
  // @param byte_count  :  int は何バイト分? (通常は 4)
  void writeAsLittleEndian(std::ostream &out, int num, unsigned int byte_count){
    for(unsigned int i=0;i<byte_count;i++){
      out.put( num & 0xff );
      num = (num>>8);
    }
  }
  // in から little-endian で get
  // @param byte_count  :  int は何バイト分? (通常は 4)
  unsigned int readAsLittleEndian(std::istream &in, unsigned int byte_count){
    unsigned int num = 0;
    int c;
    for(unsigned int i=0;i<byte_count;i++){
      if( (c = in.get()) == EOF) return num;
      num |= (c<<i*8);
    }
    return num;
  }



  /// Container
  
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
