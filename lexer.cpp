// -*- mode: cpp -*-
#include "lexer.h"

#include "util.h"

namespace nl{
  static RegEx re_lastEscape("^(((\\\\.)|[^\\\\])*)\\\\$");
  
  Lexer::Lexer(){ init(); }
  Lexer::Lexer(const std::string &file_name_){ init(); open(file_name_);}
  Lexer::Lexer(const std::string &file_name_, const std::string &content_ ){
	init();
	open(file_name_, content_);
  }
  Lexer::~Lexer(){
	if( ifs.is_open() ) ifs.close();
	is = NULL;
  }

  // ファイルを開く
  void Lexer::open(const std::string &file_name_){
	init();
	file_name = file_name_;
	ifs.open( file_name.c_str(), std::ios::in | std::ios::binary );
	if(!ifs.is_open()) ERRP("error: cannot open file " << file_name);
	else is = &ifs;
  }
  // 文字列を入力ストリームにする
  void Lexer::open(const std::string &file_name_, const std::string &content_){
	init();
	file_name = file_name_;
	ss.str(content_);
	is = &ss;
  }
  
  // トークンをひとつ切り出す。
  // @return マッチした Rule へのポインタ or NULL
  Rule *Lexer::get(){
	if( rule_list == NULL ) return ERRP("rule_list is NULL."), (Rule*)NULL;
	return get(*rule_list);
  }
  Rule *Lexer::get(RuleList &lst){
	if( !updateString()   ) return (Rule*)NULL;
	for( RuleList::iterator ite = lst.begin(); ite != lst.end(); ++ite){
	  if( ite->match( &str[idx] ) ){
		pre_idx = idx;
		idx += ite->get(0).length();
		return &(*ite);
	  }
	}
	DBGP("didn't match. " << &str[idx]);
	return NULL;
  }
  // 前回の get() を無かったことにする
  void Lexer::unget(){ idx = pre_idx; }
  
  // 読み取り中の行を返す。idx は進めない。
  const std::string &Lexer::getLine() const{ return str; }
  // 読み取り中の行の残り部分を返す。idx は進める。
  std::string Lexer::getRest(){
	int tmp = idx;
	idx = str.length();
	return &str[tmp];
  }

  // 前回返したトークンの位置情報を文字列で返す
  const char *Lexer::getPosStr() const{
	static char buf[512];
	sprintf(buf,"%s:%d:%d",file_name.c_str(),line_no, pre_idx);
	return buf;
  }

  void Lexer::init(){
	if( ifs.is_open() ) ifs.close();
	ss.str("");
	ss.clear();
	file_name = "";
	is = NULL;

	str     = "";
	line_no = idx = pre_idx =  0;
  }
  // 行末を指している? == その行で読める文字はもうない?
  bool Lexer::eol() const{ return ((str.length()-idx) <= 0); }
  // データをすべて読み終わった?
  bool Lexer::eod() const{ return (is == NULL && eol()); }
  // str を更新。
  // idx が行末まで来た
  //   -> 次の行を読む
  //      行末に \ があれば、次の行と接続。
  // @return データをすべて読み終わっていたら false
  bool Lexer::updateString(){
	// 行末じゃない -> このままでOK
	if( !eol() ) return true;
	// 行末なので getline
	idx = 0; pre_idx = 0; line_no++;
	if( is == NULL || !getline(*is, str) ){
	  // 次の行がない -> false
	  is = NULL;
	  str = "";
	  return false;
	}
	// 行末に '\' -> 次の行と繋ぐ
	while( re_lastEscape.match(str) ){
	  str = str.substr(0,str.length()-1);
	  std::string tmp;
	  line_no++;
	  if( !getline(*is, tmp) ){ is = NULL; break; }
	  str = str + tmp;
	}
	return true;
  }
	
}; // namespace nl;


#ifdef TEST
// test
// $ make TEST=lexer
#include "util.h"

using nl::RegEx;

using nl::Lexer;
using nl::Rule;
using nl::RuleList;

Lexer lexer;
RuleList rule_list;

void func(){
  Rule *ret;
  lexer.setRule(&rule_list);
  while( (ret = lexer.get()) != NULL ){
	DBGP("[" << lexer.getPosStr() << "] " << ret->type << "  '" << ret->str() << "'");
  }
  if(!lexer.eod()) DBGP("parse error.");
}

int main(){
  // タイプと正規表現のリストを作成
  rule_list.push_back( Rule("(^\\s+)|(^$)", 0,   0 )); // whitespace | 空行
  rule_list.push_back( Rule("^\\w+",        0,   1 )); // リテラル
  rule_list.push_back( Rule("^\\d+",        0,   2 )); // 数値
  rule_list.push_back( Rule("^\"(((\\\\.)|[^\"])*)\"", 1, 3 )); // 文字列
  rule_list.push_back( Rule("^//.*$",       0,   4 )); // コメント行
  rule_list.push_back( Rule("^.*$",         0, 100 )); //


  //lexer.open("TestData/input.txt");  // ファイル開く
  //func();

  //lexer.open("dat","// comment line.\nint a=0;\nint b=0;");
  lexer.open("dat","// comment line.\n");
  func();

  lexer.open("dat2","a*b");
  func();

  return 0;
}

#endif
