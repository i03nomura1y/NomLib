// -*- mode: cpp -*-
// 字句解析用
//  regex を利用。 -lregex
#ifndef __NOMLIB_LEXER_H__
#define __NOMLIB_LEXER_H__

#include <string>
#include <list>
#include <sstream> // string stream
#include <fstream> // file stream

#include "regex.h"

namespace nl{
  // クラスの宣言 中身は Lexer のあと
  class Token; // Lexer で切り出すトークンの情報
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
  
	// トークンをひとつ切り出す。
	// @return 成功したら true
	bool get(Token &token);
	void unget();	// 前回の get() を無かったことにする
	const char *getPosStr() const;	// 前回返したトークンの位置情報を文字列で返す
  private:
	void init(); // 初期化
	bool eol() const; // 行末を指している? == その行で読める文字はもうない?
	bool eod() const; // データをすべて読み終わった?
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

  // Lexer で得られたトークンの情報
  // 文字列そのもの と、タイプ(文字列、コメント、数値 など) を表す。
  class Token{
  public:
	// トークンの情報
	int type; // 正常: 0以上  無効: 負の値
	std::string str;
  public:
	// トークン情報をセット
	void set(int t, std::string s){ type = t; str = s; }
	// このトークンが'無効'を示すようにする ( 内部的には type に 負の数を入れるだけ )
	void invalidate(){ type = -1; }
  };
  
  // 字句解析ルール
  //  正規表現で表す。
  //  ユーザ定義の type は正の値。
  class Rule{
  public:
	Rule(const RegEx &re_, const unsigned int type_) : re(re_), type(type_){}
	~Rule(){}
	// コピーコンストラクタ
	Rule(const Rule &obj) : re(obj.re), type(obj.type){}
	// 代入演算子
	Rule &operator=(Rule &obj){
	  re = obj.re;
	  type = obj.type;
	  return *this;
	}
	
  public:
	RegEx re;
	unsigned int type;
  };
  
}; // namespace nl

#endif
