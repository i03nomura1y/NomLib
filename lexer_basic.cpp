// -*- mode: cpp -*-
#include "lexer_basic.h"

#include "util.h"

namespace nl{
    static RegEx re_lastEscape("^(((\\\\.)|[^\\\\])*)\\\\$");
  
    BasicLexer::BasicLexer() : is(NULL), ifs(NULL), ss(NULL){ init(); }
    BasicLexer::BasicLexer(const std::string &file_name_)
        : is(NULL), ifs(NULL), ss(NULL){
        init();
        setSourceFile(file_name_);
    }
    // コピーコンストラクタ 元のやつは壊す
    BasicLexer::BasicLexer(const BasicLexer &obj) : is(NULL), ifs(NULL), ss(NULL){
        rule_list = obj.rule_list;
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

    BasicLexer::~BasicLexer(){
        if( ifs != NULL ) delete ifs;
        if( ss  != NULL ) delete ss;
        is = NULL;
    }

    void BasicLexer::init(){
        if( ifs != NULL ) delete ifs;
        if( ss  != NULL ) delete ss;
        ifs = NULL;
        ss  = NULL;
        file_name = "";
        is = NULL;

        str     = "";
        line_no = idx = pre_idx =  0;
        line_offset = col_offset = start_col_pos = 0;
    }
    
    // 入力元として、ファイルを設定
    void BasicLexer::setSourceFile(const std::string &file_name_){
        init();
        file_name     = file_name_;
        ifs = new std::ifstream( file_name.c_str(), std::ios::in | std::ios::binary );
        if(!ifs->is_open()){ ERRP("error: cannot open file " << file_name); delete ifs; }
        else is = ifs;
    }
    // 入力元として、文字列を設定
    void BasicLexer::setSourceText(const std::string &content_){
        init();
        ss = new std::stringstream();
        ss->str(content_);
        is = ss;
    }    
    // 入力元の追加情報のセット
    void BasicLexer::setSourceInfo(const std::string &file_name_,
                              const int line_offs,
                              const int col_offs,
                              const int start_col_pos_){
        if( file_name_.length() > 0 )  file_name = file_name_;
        line_offset   = line_offs;
        col_offset    = col_offs;
        start_col_pos = start_col_pos_;
    }

    // トークンをひとつ切り出す。
    // @return マッチした LexRule へのポインタ or NULL
    LexRule *BasicLexer::get(){
        if( rule_list == NULL ) return ERRP("rule_list is NULL."), (LexRule*)NULL;
        return get(*rule_list);
    }
    LexRule *BasicLexer::get(LexRuleList &lst){
	
        if( !updateString()   ) return (LexRule*)NULL;
        for( LexRuleList::iterator ite = lst.begin(); ite != lst.end(); ++ite){
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
    void BasicLexer::unget(){ idx = pre_idx; }
  
    // 読み取り中の行を返す。idx は進めない。
    const std::string &BasicLexer::getLine() const{ return str; }
    // 読み取り中の行の残り部分を返す。idx は進める。
    std::string BasicLexer::getRest(){
        int tmp = idx;
        idx = str.length();
        return &str[tmp];
    }

    // 前回返したトークンの位置情報を文字列で返す
    const char *BasicLexer::getPosStr() const{
        static char buf[512];
        sprintf(buf,"%s:%d:%d",file_name.c_str(), getLineNo(), getColumnNo() );
        return buf;
    }


    // 行末を指している? == その行で読める文字はもうない?
    bool BasicLexer::eol() const{ return ((str.length()-idx) <= 0); }
    // データをすべて読み終わった?
    bool BasicLexer::eod() const{ return (is == NULL && eol()); }
    // str を更新。
    // idx が行末まで来た
    //   -> 次の行を読む
    //      行末に \ があれば、次の行と連結
    // @return データをすべて読み終わっていたら false
    bool BasicLexer::updateString(){
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
	
}; // namespace nl;


#ifdef TEST
// test
// $ make TEST=lexer_basic
#include "util.h"

using nl::RegEx;

using nl::BasicLexer;
using nl::LexRule;
using nl::LexRuleList;

void showAllToken(BasicLexer &lexer){
    LexRule *ret;
    DBGP("---------");
    while( (ret = lexer.get()) != NULL ){
        std::cout << "[" << lexer.getPosStr() << "] " << ret->type << "  '" << ret->str() << "'" << std::endl;
    }
    if(!lexer.eod()) DBGP("parse error.");
}

int main(){
    BasicLexer lexer;
    LexRuleList rule_list;

    // タイプと正規表現のリストを作成
    rule_list.push_back( LexRule(  0, "(^\\s+)|(^$)")); // whitespace | 空行
    rule_list.push_back( LexRule(  1, "^\\w+"       )); // リテラル
    rule_list.push_back( LexRule(  2, "^\\d+"       )); // 数値
    rule_list.push_back( LexRule(  3, "^\"(((\\\\.)|[^\"])*)\"", 1)); // 文字列
    rule_list.push_back( LexRule(  4, "^//.*$"      )); // コメント行
    rule_list.push_back( LexRule(  5, "^[\xC0-\xD6]")); // 0xC0 - 0xD6 の一文字にマッチ
    rule_list.push_back( LexRule(100, "^.*$"        )); // その他
    // lexer にセット
    lexer.setRule(&rule_list);
    
    // ファイル開く
    lexer.setSourceFile("TestData/input.txt");
    showAllToken(lexer);
    // 文字列をパース
    lexer.setSourceText("// comment line.\nint a=0;\nint b=0;");
    lexer.setSourceInfo("text01", 3, 10, 20);
    showAllToken(lexer);

    return 0;
}

#endif
