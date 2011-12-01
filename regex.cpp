// -*- mode: cpp -*-
// 参考 http://blog.majide.com/2009/03/c-regex-sample-code/
// コンパイルオプション: -lregex
#include "regex.h"

#include <stdexcept>
#include <iostream>

namespace nl{

  RegEx::RegEx(const std::string &rxstr) : str(),idx_list(){
	char errbuf[100];
	rx_str_ = rxstr;
	// regcomp の最後の引数
	//   REG_EXTENDED : POSIX拡張正規表現を使用。指定なしなら POSIX 標準正規表現が使われる
	//   REG_ICASE    : 大文字小文字の違いを無視
	//   REG_NOSUB    : regexec で パラメータ nmatch, pmatch が無視される
	//   REG_NEWLINE  : 全ての文字にマッチするオペレータに改行をマッチさせない
	int ret = regcomp(&reg, rxstr.c_str(), REG_EXTENDED | REG_NEWLINE );
	if(ret != 0){
	  regerror(ret, &reg, errbuf, sizeof errbuf);
	  fprintf(stderr, "%s(%d) regcomp error: %s\n", __FILE__,__LINE__,errbuf);
	}
	valid = (ret == 0); // コンパイルに成功したら true
	idx_list.clear();
  }
  RegEx::~RegEx(){
	idx_list.clear();
	if(valid) regfree(&reg);
  }
  bool RegEx::match(const std::string &str_){
	size_t nmatch = 100;
	regmatch_t pmatch[nmatch];
	idx_list.clear();
	str = str_;
	// regexec の最後の引数
	//   REG_NOTBOL : 行頭にマッチするオペレータは必ずマッチに失敗する
	//                (複数行文字列の先頭を行頭として解釈させない場合に用いる)
	//   REG_NOTEOL : 行末にマッチするオペレータは必ずマッチに失敗する
	if(valid && (regexec(&reg, str_.c_str() , nmatch, pmatch, 0) == REG_NOERROR) ){
	  for (unsigned int i = 0; i < nmatch && pmatch[i].rm_so>=0; i++) {
		int so = (int)pmatch[i].rm_so;
		int eo = (int)pmatch[i].rm_eo;
		//std::cout << i << " " << so << " " << eo << " " << (eo-so) << std::endl;
		idx_list.push_back( std::pair<int,int>( so, eo-so ) );
	  }
	  //std::cout << "size " << idx_list.size() << std::endl;
	  return true;
	}
	return false;
  }
  /// 前回の実行によりマッチした部分文字列の idx 番目のものを返す
  std::string RegEx::get(int idx) const{
	if( idx<0 || idx>=(int)idx_list.size() ){
	  std::cerr << __FILE__ << "(" << __LINE__ << ") error in RegEx::get(int)  size "<< idx_list.size() << "  arg " << idx << std::endl;
	  throw std::out_of_range("in RegEx::get(int)");
	}
	//std::cout << idx_list[idx].first << "," << idx_list[idx].second << std::endl;
	//std::cout << str << "," << (str.substr(0,1)) << std::endl;
	return str.substr( idx_list[idx].first, idx_list[idx].second );
  }
  int RegEx::getStartPos(int idx) const{
	if( idx<0 || idx>=(int)idx_list.size() ){
	  std::cerr << __FILE__ << "(" << __LINE__ << ") error in RegEx::getStartPos(int)  size "<< idx_list.size() << "  arg " << idx << std::endl;
	  throw std::out_of_range("in RegEx::getStartPos(int)");
	}
	return idx_list[idx].first;
  }
  int RegEx::getEndPos(int idx) const{
	if( idx<0 || idx>=(int)idx_list.size() ){
	  std::cerr << __FILE__ << "(" << __LINE__ << ") error in RegEx::getEndPos(int)  size "<< idx_list.size() << "  arg " << idx << std::endl;
	  throw std::out_of_range("in RegEx::getEndPos(int)");
	}
	return ( idx_list[idx].first + idx_list[idx].second );
  }

};

#if 1
// test
#include <iostream>
using namespace std;
using nl::RegEx;

void test(const string &rx_str, const string &str1, const string &str2){
  RegEx re(rx_str);

  if( !re.isValid() ){ cerr << "error: invalid RegEx" << endl; return; }
  
  cout << " ================================================" << endl;
  cout << "rx  : " << re.rx_str() << endl;
  cout << "str : '" << str1 << "'" << endl;
  if( re.match(str1) ){
	for(int i=0; i<re.length(); i++){
	  cout << " match[" << i << "] " << re.get(i) << endl;
	}
  }else{
	cout << " not match" << endl;
  }

  cout << "str : '" << str2 << "'" << endl;
  if( re.match(str2) ){
	for(int i=0; i<re.length(); i++){
	  cout << " match[" << i << "] " << re.get(i) << endl;
	}
  }else{
	cout << " not match" << endl;
  }
}

int main(){
  test("([[:digit:]]+), (([[:digit:]]+), ([[:digit:]]+))", 
	   "123, 456, 789", "Hello");

  test("^hello", 
	   "hello", "hello, world!");

  test("(日本語)?のテスト",
	   "日本語のテスト", "英語のテスト");
  
  return 0;
}
#endif
