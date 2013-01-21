// -*- mode:cpp -*-
// created date : 2013/01/18 23:03:41
// last updated : 2013/01/21 20:42:02
#ifndef NL_LISP_PARSER_H
#define NL_LISP_PARSER_H

#include "lexer.h"
#include "lisp.h"

#include "util.h"

namespace nl{
    class LispParser{
    private:
        LexRule Blank;   // whitespace | 空行
        LexRule Comment; // ; ...
        LexRule String;
        LexRule Number;
        LexRule Literal;
        
        std::string reg;

    public:
        LispParser(){ init_rules(); }
        ~LispParser(){}
        
        LispNode::Ptr parseFile(const std::string file_path){
            nl::Lexer lexer;
            MSGP(file_path);
            lexer.setSourceFile(file_path);
            
            LispNode::Ptr rootElem = parseDocument(lexer);
            
            /// 残りを表示
            static LexRule line = LexRule("^.+");
            if( lexer.getToken(line) ){
                DBGP("parse err: まだ残ってるよ");
                do{
                    DBGP("|" << lexer.reg());
                }while(lexer.getToken(line));
            }
            
            return rootElem;
        }
        
    private:
        LispNode::Ptr parseDocument(nl::Lexer &lexer){
            LispNode::Ptr rootNode = LispNode::buildList();
            
            LispNode::Ptr ptr;
            while(ptr = parseNode(lexer))
                rootNode->add(ptr);
            
            return rootNode;
        }

        LispNode::Ptr parseNode(nl::Lexer &lexer){
            lexer.skipToken(Blank);
            // リストの終わり: unget して false を返す
            if( lexer.getToken(")") ){ lexer.unget(); return LispNode::Ptr(); }
            if( lexer.getToken("(") ){
                LispNode::Ptr node = LispNode::buildList();
                LispNode::Ptr nd;
                while( nd = parseNode(lexer) ){
                    node->add(nd);
                }
                if( !lexer.getToken(")") ){ node->dump();  DBGP("parse error."); return LispNode::Ptr(); } // error.
                return node;
            }
            if( lexer.getToken(Comment) ) return LispNode::build(LispNode_Comment,lexer.reg() );
            if( lexer.getToken(String)  ) return LispNode::build(LispNode_String, lexer.reg() );
            if( lexer.getToken(Number)  ) return LispNode::build(LispNode_Number, lexer.reg() );
            if( lexer.getToken(Literal) ) return LispNode::build(LispNode_Literal,lexer.reg() );
            return LispNode::Ptr();
        }


        /// 正規表現のリストを作成
        void init_rules();

    };

};

#endif
