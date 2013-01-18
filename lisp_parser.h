// -*- mode:cpp -*-
// created date : 2013/01/18 23:03:41
// last updated : 2013/01/19 00:11:50
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
            if( lexer.getToken(Comment) ) return LispNode::build(LispNode_Atom, lexer.reg() );
            if( lexer.getToken(String)  ) return LispNode::build(LispNode_Atom, lexer.reg() );
            if( lexer.getToken(Number)  ) return LispNode::build(LispNode_Atom, lexer.reg() );
            if( lexer.getToken(Literal)  ) return LispNode::build(LispNode_Atom, lexer.reg() );
            return LispNode::Ptr();
        }
#if 0
        bool parseEntityRef(nl::Lexer &lexer){
            if( !lexer.getToken("&") ) return false;
            if( !parseName(lexer) ){ DBGP("parse error."); return false; } // error.
            MSGP("entity ref " << reg); // TODO
            if( !lexer.getToken(";") ){ DBGP("parse error."); return false; } // error.
            return true;
        }
        bool parsePEReference(nl::Lexer &lexer){
            if( !lexer.getToken("%") ) return false;
            if( !parseName(lexer) ){ DBGP("parse error."); return false; } // error.
            MSGP("PEReference " << reg); // TODO
            if( !lexer.getToken(";") ){ DBGP("parse error."); return false; } // error.
            return true;
        }

        bool parseAttvalue(nl::Lexer &lexer){
            if( !lexer.getToken(Quote_l) ) return false;
            // attValue の RegEx
            nl::Lexer lxr2;
            lxr2.setSourceText(lexer.reg());
            while( lxr2.getChar() ){
                if( lxr2.reg() == "<" ){ DBGP("parse error."); return false; } // error.
                else if( lxr2.reg() == "&" ){
                    if( !parseName(lxr2) ){ DBGP("parse error."); return false; } // error.
                    if( !lxr2.getToken(";") ){ DBGP("parse error."); return false; } // error.
                }
            }
            reg = lexer.reg();
            return true;
        }

        LispNode::Ptr parseCharData(nl::Lexer &lexer){
            static LexRule CharData = LexRule("(^[^<&]+)|(^$)");
            std::stringstream ss;
            
            if(lexer.eol()) ss << std::endl; // ポインタが行末に来てる -> 改行コードをいれる
            // CharData 読み出し
            bool read_succeed = false; // CharData が読めた?
            while(true){
                if( !lexer.getToken(CharData) ) break;
                read_succeed = true;
                ss << lexer.reg();
                if(lexer.eol()) ss << std::endl;
            }
            if( !read_succeed ) return LispNode::Ptr(); // 一回も読めなかった -> false
            
            if( ss.str().find("]]>") != std::string::npos ){ DBGP("parse error."); return LispNode::Ptr(); } // error
            return LispNode::buildText(ss.str());
        }

        bool parseDocTypeDecl(nl::Lexer &lexer){
            // '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
            if( !lexer.getToken("<!DOCTYPE") ) return false;
            if( !lexer.getToken(Blank) ){ DBGP("parse error."); return false; } // error.
            lexer.skipToken(Blank);
            if( !parseName(lexer) ){ DBGP("parse error."); return false; } // error.
            MSGP("DOCTYPE: [" << reg << "]"); // TODO
            lexer.skipToken(Blank);
            
            if( parseExternalID(lexer) )
                lexer.skipToken(Blank);
            if( lexer.getToken("[") ){
                if( !parseIntSubset(lexer) ){ DBGP("parse error."); return false; } // error.
                if( !lexer.getToken("]") ){ DBGP("parse error."); return false; } // error.
            }
            lexer.skipToken(Blank);
            if( !lexer.getToken(">") ){ DBGP("parse error."); return false; } // error.

            return true;
        }

        // 未実装
        bool parseElementDecl(nl::Lexer &){ return false; }
        bool parseAttlistDecl(nl::Lexer &){ return false; }
        bool parseEntityDecl(nl::Lexer &){ return false; }
        bool parseNotationDecl(nl::Lexer &){ return false; }
        
        bool parseExternalID(nl::Lexer &lexer){
            static LexRule rule0 = LexRule("^(SYSTEM)|(PUBLIC)");
            if( !lexer.getToken(rule0) ) return false;

            if( lexer.reg() == "PUBLIC" ){
                if( !lexer.getToken(Blank) ){ DBGP("parse error."); return false; } // error.
                lexer.skipToken(Blank);
                // PubidLiteral
                if( !lexer.getToken(PubidLiteral_l) ){ DBGP("parse error."); return false; } // error.
                MSGP(lexer.reg()); // TODO
            }
            if( !lexer.getToken(Blank) ){ DBGP("parse error."); return false; } // error.
            lexer.skipToken(Blank);
            // SystemLiteral
            if( !lexer.getToken(Quote_l) ){ DBGP("parse error."); return false; } // error.
            MSGP(lexer.reg()); // TODO
            return true;
        }
        bool parseIntSubset(nl::Lexer &lexer){
            // (markupdecl | DeclSep)*
            // => ( (elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment ) | ( PEReference | S) )*
            while(true){
                lexer.skipToken(Blank);
                if( parseElementDecl(lexer) ) continue;
                if( parseAttlistDecl(lexer) ) continue;
                if( parseEntityDecl(lexer) ) continue;
                if( parseNotationDecl(lexer) ) continue;
                if( parsePI(lexer) ) continue;
                if( lexer.getToken(Comment) ){ DBGP(lexer.reg()); continue; }
                if( parsePEReference(lexer) ) continue;
                break;
            }
            return true;
        }

        bool parseCDSect(nl::Lexer &lexer){
            // CDStart
            if( !lexer.getToken("<![CDATA[") ) return false;
            // CData (CDEnd までの任意の char)
            std::stringstream ss;
            while( !lexer.getToken("]]>") ){
                // char が取り出せない -> EndOfData. error
                if( !lexer.getChar() ){ DBGP("parse error."); return false; } // error.
                ss << lexer.reg();
            }
            // while を抜けた -> CDEnd が検出された。 true
            MSGP(ss.str()); // TODO
            return true;
        }
        
        // Processing instructions (PIs)
        bool parsePI(nl::Lexer &lexer){
            if( !lexer.getToken("<?") ) return false;
            static LexRule invalid_name = LexRule("^[Xx][Mm][Ll]");
            // PITarget
            if(  lexer.getToken(invalid_name) ){ DBGP("parse error."); return false; } // error.
            if( !parseName(lexer) ){ DBGP("parse error."); return false; } // error.
            MSGP("PITarget: [" << reg << "]"); // TODO

            // PITarget のみ?
            if( lexer.getToken("?>") ) return true;
            
            // S (Char* - (Char* '?>' Char*))
            if( !lexer.getToken(Blank) ){ DBGP("parse error."); return false; } // error.
            lexer.skipToken(Blank);
            std::stringstream ss;
            while( !lexer.getToken("?>") ){
                // char が取り出せない -> EndOfData. error
                if( !lexer.getChar() ){ DBGP("parse error."); return false; } // error.
                ss << lexer.reg();
            }
            // while を抜けた -> '?>' が検出された。 true
            MSGP(ss.str()); // TODO
            return true;
        }
        
        bool parseName(nl::Lexer &lexer){
            std::stringstream ss;
            if( !lexer.getToken(NameStartChar_l) ) return false;
            ss << lexer.reg();
            while( lexer.getToken(NameChar_l) )
                ss << lexer.reg();
            reg = ss.str();
            return true;
        }
#endif        

        /// 正規表現のリストを作成
        void init_rules();

    };

};

#endif
