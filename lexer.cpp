// -*- mode: cpp -*-
#include "lexer.h"

#include <algorithm>
#include "util.h"

namespace nl{
    
    Lexer::~Lexer(){}

#if 0
    LexRule *Lexer::getToken(){
        return (reg_tkn = get());
#if 0
        // token.type が skip_types に含まれている間ループ
        while( (reg_tkn = get()) &&
               std::find( skip_types.begin(), skip_types.end(), reg_tkn->type) != skip_types.end());
        return reg_tkn;
#endif
    }
#endif
}; // namespace nl;


#ifdef TEST
// test
// $ make TEST=lexer
#include "util.h"

using nl::RegEx;

using nl::Lexer;
using nl::LexRule;
using nl::LexRuleList;

void showAllToken(Lexer &lexer){
    LexRule *ret;
    DBGP("---------");
    while( (ret = lexer.get()) != NULL ){
        std::cout << "[" << lexer.getPosStr() << "] " << ret->type << "  '" << ret->str() << "'" << std::endl;
    }
    if(!lexer.eod()) DBGP("parse error.");
}

int main(){
    Lexer lexer;
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
