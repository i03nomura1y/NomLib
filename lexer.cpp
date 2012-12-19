// -*- mode: cpp -*-
#include "lexer.h"

#include "util.h"

namespace nl{
    static RegEx re_lastEscape("^(((\\\\.)|[^\\\\])*)\\\\$");

    Lexer::Lexer() : is(NULL), ifs(NULL), ss(NULL){ init(); }
    Lexer::Lexer(const std::string &file_name_)
        : is(NULL), ifs(NULL), ss(NULL){
        init();
        setSourceFile(file_name_);
    }
    // コピーコンストラクタ 元のやつは壊す
    Lexer::Lexer(const Lexer &obj) : is(NULL), ifs(NULL), ss(NULL){
        str       = obj.str      ;
        line_no   = obj.line_no  ;
        idx 	  = obj.idx 	 ;
        pre_idx   = obj.pre_idx  ;
        file_name = obj.file_name;
        swap(ifs, obj.ifs);
        swap(ss , obj.ss );
        is            = obj.is;
        line_offset   = obj.line_offset;
        col_offset    = obj.col_offset;
        start_col_pos = obj.start_col_pos;
    }

    Lexer::~Lexer(){
        if( ifs != NULL ) delete ifs;
        if( ss  != NULL ) delete ss;
        is = NULL;
    }

    void Lexer::init(){
        if( ifs != NULL ) delete ifs;
        if( ss  != NULL ) delete ss;
        ifs = NULL;
        ss  = NULL;
        file_name = "";
        is = NULL;
        reg_str = "";

        str     = "";
        line_no = idx = pre_idx =  0;
        line_offset = col_offset = start_col_pos = 0;
    }
    
    // 入力元として、ファイルを設定
    void Lexer::setSourceFile(const std::string &file_name_){
        init();
        file_name     = file_name_;
        ifs = new std::ifstream( file_name.c_str(), std::ios::in | std::ios::binary );
        if(!ifs->is_open()){ ERRP("error: cannot open file " << file_name); delete ifs; }
        else is = ifs;
    }
    // 入力元として、文字列を設定
    void Lexer::setSourceText(const std::string &content_){
        init();
        ss = new std::stringstream();
        ss->str(content_);
        is = ss;
    }    
    // 入力元の追加情報のセット
    void Lexer::setSourceInfo(const std::string &file_name_,
                              const int line_offs,
                              const int col_offs,
                              const int start_col_pos_){
        if( file_name_.length() > 0 )  file_name = file_name_;
        line_offset   = line_offs;
        col_offset    = col_offs;
        start_col_pos = start_col_pos_;
    }

    // トークンをひとつ切り出す。
    bool Lexer::getToken(LexRule &tkn){
        if( !updateString()   ) return false;
        if( !tkn.match( &str[idx] ) ) return false;
        incCursor( tkn.get(0).length() );
        reg_str = tkn.str();
        return true;
    }
    // @return マッチした: true / マッチしなかった: false
    bool Lexer::getToken(const std::string &tkn_){
        if( !updateString()   ) return false;
        int t_len = tkn_.length();        // token length
        int s_len = str.length() - idx;   // 残りstr length
        if( s_len < t_len ) return false; //  (残りstr < token) -> false
        for(int i=0;i<t_len;i++)
            if( tkn_[i] != str[i+idx] ) return false;
        incCursor( tkn_.length() );
        reg_str = tkn_;
        return true;
    }

#if 0
    // 最初にマッチしたやつを返す
    LexRule *Lexer::getToken(std::list<LexRule> &lst){
        if( !updateString()   ) return (LexRule*)NULL;
        for( std::list<LexRule>::iterator ite = lst.begin(); ite != lst.end(); ++ite){
            if( getToken(*ite) ) return &(*ite);
        }
        return NULL;
    }
#endif
    // 任意の一文字
    bool Lexer::getChar(){
        reg_str = "";
        if( !updateString()   ) return false;
        reg_str = str[idx];
        incCursor(1);
        return true;
    }
        
    // 前回の get() を無かったことにする
    void Lexer::unget(){ idx = pre_idx; }

    // 読み取り中の行の残り部分を返す。idx は進める。
    std::string Lexer::popRestStr(){
        int tmp = idx;
        idx = str.length();
        return &str[tmp];
    }

    // 前回返したトークンの位置情報を文字列で返す
    const char *Lexer::prePosStr() const{
        static char buf[512];
        sprintf(buf,"%s:%d:%d",file_name.c_str(), currentLineNo(), preColumnNo() );
        return buf;
    }


    // 行末を指している? == その行で読める文字はもうない?
    bool Lexer::eol() const{ return ((str.length()-idx) <= 0); }
    // データをすべて読み終わった?
    bool Lexer::eod() const{ return (is == NULL && eol()); }
    // str を更新。
    // idx が行末まで来た
    //   -> 次の行を読む
    //      行末に \ があれば、次の行と連結
    // @return データをすべて読み終わっていたら false
    bool Lexer::updateString(){
        // 行末じゃない -> このままでOK
        if( !eol() ) return true;
        // 行末なので getline
        idx = 0; pre_idx = 0;
        if( line_no > 0 ) start_col_pos = col_offset; // 最初の getline では更新しない。
        line_no++;
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

    // カーソルを n 文字分進める
    void Lexer::incCursor(unsigned int n){
        pre_idx = idx;
        idx += n;
    }
  
	
}; // namespace nl;


#ifdef TEST
// test
// $ make TEST=lexer_basic
#include "util.h"

using nl::RegEx;

using nl::Lexer;
using nl::LexRule;
using nl::LexRuleList;

void showAllToken(Lexer &lexer,LexRuleList &rule_list){
    LexRule *ret;
    while( (ret = lexer.getToken(rule_list)) != NULL ){
        std::cout << "[" << lexer.prePosStr() << "] " << "  '" << ret->str() << "'" << std::endl;
    }
    if(!lexer.eod()) DBGP("parse error.");
}

int main(){
    Lexer lexer;
    LexRuleList rule_list;

    // タイプと正規表現のリストを作成
    // rule_list.push_back( LexRule(  0, "(^\\s+)|(^$)")); // whitespace | 空行
    // rule_list.push_back( LexRule(  1, "^\\w+"       )); // リテラル
    // rule_list.push_back( LexRule(  2, "^\\d+"       )); // 数値
    // rule_list.push_back( LexRule(  3, "^\"(((\\\\.)|[^\"])*)\"", 1)); // 文字列
    // rule_list.push_back( LexRule(  4, "^//.*$"      )); // コメント行
    // rule_list.push_back( LexRule(  5, "^[\xC0-\xD6]")); // 0xC0 - 0xD6 の一文字にマッチ
    // rule_list.push_back( LexRule(100, "^.*$"        )); // その他
    rule_list.push_back( LexRule("(^\\s+)|(^$)")); // whitespace | 空行
    rule_list.push_back( LexRule("^\\w+"       )); // リテラル
    rule_list.push_back( LexRule("^\\d+"       )); // 数値
    rule_list.push_back( LexRule("^\"(((\\\\.)|[^\"])*)\"", 1)); // 文字列
    rule_list.push_back( LexRule("^//.*$"      )); // コメント行
    rule_list.push_back( LexRule("^[\xC0-\xD6]")); // 0xC0 - 0xD6 の一文字にマッチ
    rule_list.push_back( LexRule("^.*$"        )); // その他
    
    // ファイル開く
    DBGP("------------------------");
    lexer.setSourceFile("TestData/input.txt");
    showAllToken(lexer, rule_list);

    // 文字列をパース
    DBGP("------------------------");
    lexer.setSourceText("// comment line.\nint a=0;\nint b=0;");
    lexer.setSourceInfo("text01", 3, 10, 20);
    showAllToken(lexer, rule_list);

    //
    DBGP("------------------------");
    lexer.setSourceFile("TestData/simple.xml");
    lexer.getToken("<?xml");
    DBGP(lexer.popRestStr());

    return 0;
}

#endif
