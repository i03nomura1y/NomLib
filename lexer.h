// -*- mode: cpp -*-
// 字句解析用
//  regex を利用。 -lregex
// class:
//   - Lexer
//
//   - LexRule
//   - LexRuleList  <- typedef
#ifndef __NOMLIB_LEXER_BASIC_H__
#define __NOMLIB_LEXER_BASIC_H__

#include <string>
#include <list>
#include <sstream> // string stream
#include <fstream> // file stream

#include "regex.h"

namespace nl{
    class Lexer;
    // クラスの宣言 中身は Lexer のあと
    class LexRule;  // 切り出しのルール
    typedef std::list<LexRule> LexRuleList;
    
    // 入力ファイル/文字列 からトークンを切り出す
    class Lexer{
    public:
        Lexer();
        explicit Lexer(const std::string &file_name_);
        virtual ~Lexer();
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

        /// トークンを切り出す。
        // @return マッチした: true / マッチしなかった: false
        bool getToken(LexRule &tkn);
        bool getToken(const std::string &tkn_);
        // @return 最初にマッチした token へのポインタ or NULL
        template<class T>
        T *getToken(std::list<T> &lst){
            for( std::list<LexRule>::iterator ite = lst.begin(); ite != lst.end(); ++ite){
                if( getToken(*ite) ) return &(*ite);
            }
            return NULL;
        }
        // 任意の一文字
        bool getChar();
        // getToken で前回返した token 文字列
        std::string reg() const{ return reg_str; }
        
        /// 前回の get() を無かったことにする
        void unget();

        /// 条件にマッチする token を読み飛ばす。
        // T: LexRule, string, std::list<LexRule>, std::list<string>
        template<class T>
        void skipToken(T &val){ while( getToken(val) ); }

        /// 前回返したトークンの情報を返す
        const char *prePosStr() const; // 位置情報 を文字列で返す

        bool eod() const; // データをすべて読み終わった?
    private:
        // 読み取り中の行を返す。idx は進めない。
        const std::string &currentLine() const{ return str; }
        std::string popRestStr();  // 読み取り中の行の残り部分を返す。idx は進める。
        
        // 現在の行/列番号の取得
        int currentLineNo()       const{ return line_no   + line_offset;      }
        int currentColumnNo() const{ return idx    +1 + start_col_pos;        } // 列番号は 1 から始まる
        int preColumnNo()     const{ return pre_idx+1 + start_col_pos; } // 前回返したトークンの開始位置
        const std::string &fileName() const{ return file_name; }
	
        bool eol() const; // 行末を指している? == その行で読める文字はもうない?
        
    private:
        void init(); // 初期化
        // str を更新。
        // idx が行末まで来た
        //   -> 次の行を読む
        //      行末に \ があれば、次の行と接続。
        // @return データをすべて読み終わっていたら false
        bool updateString();
        // カーソルを n 文字分進める
        void incCursor(unsigned int n);
	
    private:
        // File の内容
        std::string str; // カーソルがある行の文字列
        // カーソル
        int line_no; // 現在の str が何行目のものか
        int idx;     // str の何文字目までスキャンしたか
        int pre_idx; // 前回 get() が呼ばれたときの idx (unget用)

        std::string reg_str; // getToken で前回返した token
        
        // File にあたる部分
        std::string file_name;
        std::istream  *is; // ポインタ. ifs / ss を指す
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
        LexRule(const std::string &rxstr, unsigned int idx_ = 0) : RegEx(rxstr), type(0), idx(idx_){}
        ~LexRule(){}
        LexRule(const LexRule &obj) : RegEx(obj), type(obj.type), idx(obj.idx){} // コピーコンストラクタ

        // type / str() が等しい?
        bool operator==(const unsigned int &type_) const{ return type == type_; }
        bool operator==(const std::string  &str_ ) const{ return str() == str_; }
        template<class T>
        bool equals(const T &val) const{ return operator==(val); }
        // list で type / str() を指定
        template<class T>
        bool equals(const std::list<T> &vals) const{
            return ( std::find(vals.begin(), vals.end(), *this) != vals.end() );
        }
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
