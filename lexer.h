// -*- mode: cpp -*-
// 字句解析用
//  regex を利用。 -lregex
// class:
//   - Lexer
//   - Rule
//   - RuleList  <- typedef
#ifndef __NOMLIB_LEXER_H__
#define __NOMLIB_LEXER_H__

#include <string>
#include <list>
#include <sstream> // string stream
#include <fstream> // file stream

#include "regex.h"

namespace nl{
  // クラスの宣言 中身は Lexer のあと
  class Rule;  // 切り出しのルール
  typedef std::list<Rule> RuleList;
  
  // 登録されたルールに従って、入力ファイル/文字列 からトークンを切り出す
  class Lexer{
  public:
	Lexer();
	explicit Lexer(const std::string &file_name_);
	Lexer(const std::string &file_name_, const std::string &content_ );
	~Lexer();
  private:
	Lexer(const Lexer &obj){}	// コピーコンストラクタ
	Lexer &operator=(Lexer &obj){ return *this; } // 代入演算子
  public:
	// ファイルを開く
	void open(const std::string &file_name_);
	// 文字列を入力ストリームにする
	void open(const std::string &file_name_, const std::string &content_);
	// ルールをセット
	void setRule( RuleList *rule_list_ ){ rule_list = rule_list_; }
  
	// 現在の行/列番号
	int getLineNo() const{ return line_no; }
	int getColumnNo() const{ return pre_idx; }

	// トークンをひとつ切り出す。
	// @return マッチした Rule へのポインタ or NULL
	Rule *get(); // RuleList は setRule で渡されたものを使う
	Rule *get(RuleList &lst); // 渡した RuleList を使う。 rule_list は更新しない。
	// 読み取り中の行の残り部分を返す。idx は進める。
	std::string getRest();
	/// 前回返したトークンの情報を返す
	const char *getPosStr() const; // 位置情報 を文字列で返す

	void unget();	// 前回の get() を無かったことにする
	bool eod() const; // データをすべて読み終わった?
  private:
	void init(); // 初期化
	bool eol() const; // 行末を指している? == その行で読める文字はもうない?
	// str を更新。
	// idx が行末まで来た
	//   -> 次の行を読む
	//      行末に \ があれば、次の行と接続。
	bool updateString();

  private:
	RuleList *rule_list;
  
	// File の内容
	std::string str;
	int line_no; // 現在の str が何行目のものか
	int idx;     // str の何文字目までスキャンしたか
	int pre_idx; // 前回 get() が呼ばれたときの idx (unget用)
	// File にあたる部分
	std::string file_name;
	std::istream *is;
	std::ifstream ifs;
	std::stringstream ss;
  };
  
  // 字句解析ルール
  //  正規表現で表す。
  //  ユーザ定義の type は正の値。
  class Rule : public RegEx{
  public:
	Rule(const std::string &rxstr, const unsigned int type_) : RegEx(rxstr), type(type_){}
	~Rule(){}
	Rule(const Rule &obj) : RegEx(obj), type(obj.type){}	// コピーコンストラクタ
	Rule &operator=(Rule &obj){ swap(obj); return *this; }	// 代入演算子
	void swap(Rule &obj){ RegEx::swap(obj); std::swap(type, obj.type); }
	
  public:
	unsigned int type;
  };
  
}; // namespace nl

#endif
