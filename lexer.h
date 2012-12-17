// -*- mode: cpp -*-
// 字句解析用
//  regex を利用。 -lregex
//  BasicLexer を継承した Lexer クラス
// class:
//   - Lexer
#ifndef __NOMLIB_LEXER_H__
#define __NOMLIB_LEXER_H__

#include <string>
#include <list>
#include <sstream> // string stream
#include <fstream> // file stream

#include "lexer_basic.h"

namespace nl{
#if 0
    class Lexer;
    
    // 登録されたルールに従って、入力ファイル/文字列 からトークンを切り出す
    class Lexer : public BasicLexer{
    public:
        virtual ~Lexer();
        
        // token を返す。
        LexRule *getToken();
        
        // 切り出した token が条件(type/str())にマッチしていたら、その token を返す。
        // マッチしてなかったら unget して NULL を返す。
        // std::list で複数の条件を指定可(or)。
        template<class T>
        LexRule *getToken(const T &val){
            if( !getToken() ) return NULL;
            if( reg_tkn->equals(val) ) return reg_tkn;
            unget();
            return NULL;
        }
        
        // getToken で前回返した token 文字列
        std::string reg(){ return reg_tkn ? reg_tkn->str() : ""; }
        
        // 条件にマッチする token を読み飛ばす。
        template<class T>
        void skipToken(const T &val){
            while( getToken(val) );
        }
        
    private:
        LexRule *reg_tkn; // getToken で前回返した token
    };
#endif
  
}; // namespace nl

#endif
