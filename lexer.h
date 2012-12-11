// -*- mode: cpp -*-
// 字句解析用
//  regex を利用。 -lregex
// class:
//   - Lexer
//   - LexRule
//   - LexRuleList  <- typedef
#ifndef __NOMLIB_LEXER_H__
#define __NOMLIB_LEXER_H__

#include <string>
#include <list>
#include <sstream> // string stream
#include <fstream> // file stream

#include "regex.h"
#include "util.h"

namespace nl{
    class Lexer;
    // クラスの宣言 中身は Lexer のあと
    class LexRule;  // 切り出しのルール
    typedef std::list<LexRule> LexRuleList;
    
    // 登録されたルールに従って、入力ファイル/文字列 からトークンを切り出す
    class Lexer{
    public:
        Lexer();
        explicit Lexer(const std::string &file_name_);
        ~Lexer();
        Lexer(const Lexer &); // コピーコンストラクタ 元のやつは壊す
    private:
        Lexer &operator=(Lexer &){ return *this; } // 代入演算子
    public:
        /// 入力元の設定
        void setSourceFile(const std::string &file_name_);
        void setSourceText(const std::string &content_);
        // 入力元の追加情報のセット
        // file_name_ は、 "" を渡すと file_name を更新しない。
        void setSourceInfo(const std::string &file_name_,
                           const int line_offs = 0,        // line の初期位置
                           const int col_offs  = 0,        // col のオフセット
                           const int start_col_pos_  = 0); // col の初期位置

        // ルールをセット
        void setRule( LexRuleList *rule_list_ ){ rule_list = rule_list_; }
        
        // トークンをひとつ切り出す。
        // @return マッチした LexRule へのポインタ or NULL
        LexRule *get(); // LexRuleList は setRule で渡されたものを使う
        LexRule *get(LexRuleList &lst); // 渡した LexRuleList を使う。 rule_list は更新しない。
        // 前回の get() を無かったことにする
        void unget();
        
        // 切り出した token が条件にマッチしていたら、その token を返す。
        // マッチしてなかったら unget して NULL を返す。
        LexRule *getIf(const unsigned int type_); // type を指定
        LexRule *getIf(const std::string &str_ ); // 文字列 を指定
        // 指定typeのあいだ読み捨て。一回もtypeが切り出せなかったら false
        bool getWhile(const unsigned int type_);

        const std::string &getLine() const;  // 読み取り中の行を返す。idx は進めない。
        int getRawColumnNo() const{ return pre_idx; }  // pre_idx
        std::string getRest();	// 読み取り中の行の残り部分を返す。idx は進める。
        
        // 現在の行/列番号の取得
        int getLineNo()       const{ return line_no   + line_offset  ; }
        int getColumnNo()     const{ return pre_idx+1 + start_col_pos; } // 列番号は 1 から始まる
        int getNextColumnNo() const{ return idx    +1 + start_col_pos; } //  今から読むトークンの開始位置
        const std::string &getFileName() const{ return file_name; }
	
        /// 前回返したトークンの情報を返す
        const char *getPosStr() const; // 位置情報 を文字列で返す

        bool eol() const; // 行末を指している? == その行で読める文字はもうない?
        bool eod() const; // データをすべて読み終わった?
    private:
        void init(); // 初期化
        // str を更新。
        // idx が行末まで来た
        //   -> 次の行を読む
        //      行末に \ があれば、次の行と接続。
        // @return データをすべて読み終わっていたら false
        bool updateString();
	
    private:
        LexRuleList *rule_list;
	
        // File の内容
        std::string str;
        int line_no; // 現在の str が何行目のものか
        int idx;     // str の何文字目までスキャンしたか
        int pre_idx; // 前回 get() が呼ばれたときの idx (unget用)
        // File にあたる部分
        std::string file_name;
        std::istream  *is;
        mutable std::ifstream *ifs;
        mutable std::stringstream *ss;
	
        int line_offset;   // オフセット
        int col_offset;    // 
        int start_col_pos; // col の初期位置
    };
  
    // 字句解析ルール
    //  正規表現で表す。
    //  ユーザ定義の type は正の値。
    class LexRule : public RegEx{
    public:
        // @param type_  識別用
        // @param rxstr  正規表現 文字列
        // @param idx    何番目のマッチを str() で返すか
        LexRule(unsigned int type_, const std::string &rxstr, unsigned int idx_ = 0) : RegEx(rxstr), type(type_), idx(idx_){}
        ~LexRule(){}
        LexRule(const LexRule &obj) : RegEx(obj), type(obj.type), idx(obj.idx){} // コピーコンストラクタ
    private:
        LexRule &operator=(LexRule &){ return *this; }    // 代入演算子 禁止
    public:
        std::string str() const{ return ( idx<length() )?get(idx):""; } // マッチ部分の文字列
    public:
        const unsigned int type;
        const unsigned int idx;
    };
  
}; // namespace nl

#endif
