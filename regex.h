// -*- mode: cpp -*-
// regex.h の c++ 版
// -lregex
#ifndef __NOMLIB_REGEX_H__
#define __NOMLIB_REGEX_H__

#include <regex.h>
#include <string>
#include <vector>

namespace nl{
  class RegEx{
  public:
	// 正規表現文字列をコンパイルする
	// @param rxstr 正規表現 文字列
	explicit RegEx(const std::string &rxstr);
	~RegEx();
	// コンストラクタに渡された文字列を返す
	std::string rx_str() const{ return rx_str_; };
	// 正規表現を正常にコンパイルできたか？
	bool isValid() const { return valid; }
	
	/// 正規表現にマッチする?
	// @return  マッチしたら true
	bool match(const std::string &str_);
	/// 前回の実行によりマッチした部分文字列の idx 番目のものを返す
	std::string get(int idx) const; // 部分文字列
	int getStartPos(int idx) const; // 開始インデクス
	int getEndPos(int idx) const;   // 終了インデクス
	// 部分文字列の個数
	int length() const{ return idx_list.size(); }
	
  private:
	bool valid;
	regex_t reg;
	std::string rx_str_; // コンストラクタに渡された文字列
	// 前回 match()実行したときの文字列
	std::string str;
	// 前回の match()実行によりマッチした部分文字列の(インデクス,長さ) ただし最大100個
	std::vector<std::pair<int,int> > idx_list;
	
  };
}; // namespace nl

#endif

