// -*- mode:cpp -*-
#ifndef NL_XML_PARSER_H
#define NL_XML_PARSER_H

#include "lexer.h"
#include "xml.h"

#include "util.h"

namespace nl{
    // http://www.w3.org/TR/xml/
    class XmlParser{
    private:
        LexRule Blank;  // whitespace | 空行
        LexRule Comment; // <!--  -->
        LexRuleList Quote_l;
        LexRuleList PubidLiteral_l;
        LexRuleList NameStartChar_l;
        LexRuleList NameChar_l;
        LexRuleList CharRef_l;

        std::string reg;

    public:
        XmlParser(){
            init_rules();
        }
        ~XmlParser(){}

        
        XmlNode::Ptr parseFile(const std::string file_path){
            nl::Lexer lexer;
            MSGP(file_path);
            lexer.setSourceFile(file_path);
            
            XmlNode::Ptr rootElem = parseDocument(lexer);
            
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
        XmlNode::Ptr parseDocument(nl::Lexer &lexer){
            // document ::= prolog element Misc*
            
            /// prolog ::= XMLDecl?  Misc*  (doctypedecl Misc*)?
            parseXMLDecl(lexer);
            while( parseMisc(lexer) );
            if( parseDocTypeDecl(lexer) )
                while( parseMisc(lexer) );
            ///
            XmlNode::Ptr rootElem = parseElement(lexer);
            ///
            while( parseMisc(lexer) );
            
            return rootElem;
        }

        void parseXMLDecl(nl::Lexer &lexer){
            static LexRule VerNum        = LexRule("^1\\.[0-9]+"); // Version Number
            static LexRule EncName       = LexRule("^[A-Za-z][-A-Za-z0-9._]*"); // Encode Name
            static LexRule StandAloneVal = LexRule("^(yes)|(no)");
            
            if( !lexer.getToken("<?xml") ) return;
            if( !lexer.getToken(Blank) ){ DBGP("parse error."); return; } // error.
            
            // VersionInfo
            lexer.skipToken(Blank);
            if( !lexer.getToken("version") ){ DBGP("parse error."); return; } // error.
            lexer.skipToken(Blank);
            if( !lexer.getToken("=") ){ DBGP("parse error."); return; } // error.
            lexer.skipToken(Blank);
            // VerNum
            if( !lexer.getToken(Quote_l)){ DBGP("parse error."); return; } // error.
            if( !VerNum.match(lexer.reg()) ){ DBGP("parse error."); return; } // error.
            MSGP("VersionNum: [" << lexer.reg() << "]");
            
            // EncodingDecl?
            lexer.skipToken(Blank);
            if( lexer.getToken("encoding") ){
                lexer.skipToken(Blank);
                if( !lexer.getToken("=") ){ DBGP("parse error."); return; } // error.
                lexer.skipToken(Blank);
                // EncName
                if( !lexer.getToken(Quote_l)){ DBGP("parse error."); return; } // error.
                if( !EncName.match(lexer.reg()) ){ DBGP("parse error."); return; } // error.
                MSGP("EncodingName: [" << lexer.reg() << "]");
            }
            // SDDecl?
            lexer.skipToken(Blank);
            if( lexer.getToken("standalone") ){
                lexer.skipToken(Blank);
                if( !lexer.getToken("=") ){ DBGP("parse error."); return; } // error.
                lexer.skipToken(Blank);
                if( !lexer.getToken(Quote_l)){ DBGP("parse error."); return; } // error.
                if( !StandAloneVal.match(lexer.reg()) ){ DBGP("parse error."); return; } // error.
                MSGP("standalone: [" << lexer.reg() << "]");
            }
            lexer.skipToken(Blank);
            if( !lexer.getToken("?>") ){ DBGP("parse error."); return; } // error.
        }

        bool parseMisc(nl::Lexer &lexer){
            lexer.skipToken(Blank);
            if( lexer.getToken(Comment) ) return true;
            if( parsePI(lexer) ) return true;
            return false;
        }

        XmlNode::Ptr parseElement(nl::Lexer &lexer){
            // ETag のはじまり -> unget してfalse を返す
            if( lexer.getToken("</") ){ lexer.unget(); return XmlNode::Ptr(); }
            
            // STag / EmptyElemTag
            if( !lexer.getToken("<") ) return XmlNode::Ptr();
            if( !parseName(lexer) ){ DBGP("parse error."); return XmlNode::Ptr(); } // error.
            XmlNode::Ptr node = XmlNode::build(reg);
            // Attribute
            XmlAttr::Ptr attr;
            while( attr = parseAttribute(lexer) ){
                node->attr_list.push_back(attr);
            }
            lexer.skipToken(Blank);
            if( lexer.getToken("/>") ) return node; // EmptyElemTag = 中身のないタグ
            
            if( !lexer.getToken(">") ){ DBGP("parse error."); return XmlNode::Ptr(); } // error.
            
            /// content ::= CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*
            XmlNode::Ptr ptr;
            if( ptr = parseCharData(lexer) ){
                node->add(ptr);
            }
            while(true){
                if      ( lexer.getToken(CharRef_l) ){
                    MSGP(lexer.reg());
                }else if( parseEntityRef(lexer) ){
                }else if( parseCDSect(lexer) ){
                }else if( parsePI(lexer) ){
                }else if( lexer.getToken(Comment) ){
                    MSGP(lexer.reg());
                }else if( ptr = parseElement(lexer) ){
                    node->add(ptr);
                }else{
                    break;
                }
                if( ptr = parseCharData(lexer) ){
                    node->add(ptr);
                }
            }
            // ETag
            if( !lexer.getToken("</") ){ DBGP("parse error."); return XmlNode::Ptr(); } // error.
            if( !parseName(lexer) ){ DBGP("parse error."); return XmlNode::Ptr(); } // error.
            lexer.skipToken(Blank);
            if( !lexer.getToken(">") ){ DBGP("parse error."); return XmlNode::Ptr(); } // error.
            
            // 閉じタグが違う -> エラー
            if(reg != node->name){ DBGP("parse error."); return XmlNode::Ptr(); } // error.
            
            return node;
        }
        
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

        XmlAttr::Ptr parseAttribute(nl::Lexer &lexer){
            lexer.skipToken(Blank);
            if( !parseName(lexer) ) return XmlAttr::Ptr();
            XmlAttr::Ptr attr = XmlAttr::build();
            attr->name = reg;
            lexer.skipToken(Blank);
            if( !lexer.getToken("=") ){ DBGP("parse error."); return XmlAttr::Ptr(); } // error.
            lexer.skipToken(Blank);
            if( !parseAttvalue(lexer) ){ DBGP("parse error."); return XmlAttr::Ptr(); } // error.
            attr->value = reg;
            return attr;
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

        XmlNode::Ptr parseCharData(nl::Lexer &lexer){
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
            if( !read_succeed ) return XmlNode::Ptr(); // 一回も読めなかった -> false
            
            if( ss.str().find("]]>") != std::string::npos ){ DBGP("parse error."); return XmlNode::Ptr(); } // error
            return XmlNode::buildText(ss.str());
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
        

        /// 正規表現のリストを作成
        void init_rules(){
            static bool initialized = false;
            if(initialized) return;
            
            Blank   = LexRule("(^\\s+)|(^$)");
            Comment = LexRule("^<!--([^-]|(-[^-]))*-->", 1);

            // 文字列
            Quote_l.push_back( LexRule("^'(((\\\\.)|[^'])*)'",1 ));
            Quote_l.push_back( LexRule("^\"(((\\\\.)|[^\"])*)\"",1 ));
            // PubidLiteral
            PubidLiteral_l.push_back( LexRule("^'([-#x20#x0D#x0Aa-zA-Z0-9()+,./:=?;!*#@$_%]*)'",1 ));
            PubidLiteral_l.push_back( LexRule("^\"([-#x20#x0D#x0Aa-zA-Z0-9()+,./:=?;!*#@$_%']*)\"",1 ));
            // NameStartChar
            NameStartChar_l.push_back( LexRule("^[:A-Z_a-z]") );
            NameStartChar_l.push_back( LexRule("^\xC3[\x80-\x96\x98-\xB6\xB8-\xBF]") ); // Unicode: C0-D6 | D8-F6 | F8-FF
            NameStartChar_l.push_back( LexRule("^[\xC4-\xCB][\x80-\xBF]") ); // Unicode: 0100-02FF
            NameStartChar_l.push_back( LexRule("^\xCD[\xB0-\xBD\xBF]") );    // Unicode: 0370-037D | 037F
            NameStartChar_l.push_back( LexRule("^[\xCE-\xDF][\x80-\xBF]") ); // Unicode: 0380-07FF
            NameStartChar_l.push_back( LexRule("^\xE0[\xA0-\xBF][\x80-\xBF]") ); // Unicode: 0800-0FFF
            NameStartChar_l.push_back( LexRule("^\xE1[\x80-\xBF][\x80-\xBF]") ); // Unicode: 1000-1FFF
            NameStartChar_l.push_back( LexRule("^\xE2\x80[\x8C-\x8D]") );         // Unicode: 200C-200D
            NameStartChar_l.push_back( LexRule("^\xE2\x81[\xB0-\xBF]") );         // Unicode: 2070-207F
            NameStartChar_l.push_back( LexRule("^\xE2[\x82-\x85][\x80-\xBF]") );  // Unicode: 2080-
            NameStartChar_l.push_back( LexRule("^\xE2\x86[\x80-\x8F]") );         // Unicode:     -218F
            NameStartChar_l.push_back( LexRule("^\xE2[\xB0-\xBE][\x80-\xBF]") );  // Unicode: 2C00-2FBF
            NameStartChar_l.push_back( LexRule("^\xE2\xBF[\x80-\xAF]") );         // Unicode: 2FC0-2FEF
            //NameStartChar_l.push_back( LexRule("^\xE3\x80\x80") );                      // Unicode: 3000 (NameStartCharではない)
            //NameStartChar_l.push_back( LexRule("^[\xE3-\xEC][\x80-\xBF][\x80-\xBF]") ); // Unicode: 3001-CFFF
            NameStartChar_l.push_back( LexRule("^\xE3\x80[\x81-\xBF]") ); // Unicode: 3001-
            NameStartChar_l.push_back( LexRule("^\xE3[\x81-\xBF][\x80-\xBF]") ); // Unicode: 
            NameStartChar_l.push_back( LexRule("^[\xE4-\xEC][\x80-\xBF][\x80-\xBF]") ); // Unicode: -CFFF
            NameStartChar_l.push_back( LexRule("^\xED[\x80-\x9F][\x80-\xBF]") );        // Unicode: D000-D7FF
            NameStartChar_l.push_back( LexRule("^\xEF[\xA4-\xB6][\x80-\xBF]") );   // Unicode: F900-FDBF
            NameStartChar_l.push_back( LexRule("^\xEF\xB7[\x80-\x8F\xB0-\xBF]") ); // Unicode: FDC0-FDCF | FDF0-FDFF
            NameStartChar_l.push_back( LexRule("^\xEF[\xB8-\xBE][\x80-\xBF]") );   // Unicode: FE00-FFBF
            NameStartChar_l.push_back( LexRule("^\xEF\xBF[\x80-\xBD]") );          // Unicode: FFC0-FFFD
            NameStartChar_l.push_back( LexRule("^\xF0[\x90-\xBF][\x80-\xBF][\x80-\xBF]") );        // Unicode: 010000-03FFFF
            NameStartChar_l.push_back( LexRule("^[\xF1-\xF2][\x80-\xBF][\x80-\xBF][\x80-\xBF]") ); // Unicode: 040000-0BFFFF
            NameStartChar_l.push_back( LexRule("^\xF3[\x80-\xAF][\x80-\xBF][\x80-\xBF]") );        // Unicode: 0C0000-0EFFFF
            // NameChar
            NameChar_l.insert( NameChar_l.begin(), NameStartChar_l.begin(), NameStartChar_l.end() );
            NameChar_l.push_back( LexRule("^[-.0-9\xB7]") );
            NameChar_l.push_back( LexRule("^\xCC[\x80-\xBF]") ); // Unicode: 0300-033F
            NameChar_l.push_back( LexRule("^\xCD[\x80-\xAF]") ); // Unicode: 0340-036F
            NameChar_l.push_back( LexRule("^\xE2\x8F\xBF") ); // Unicode: 203F
            NameChar_l.push_back( LexRule("^\xE2\x90\x80") ); // Unicode: 2040
            // CharRef
            CharRef_l.push_back( LexRule("^&#[0-9]+;") );
            CharRef_l.push_back( LexRule("^&#x[0-9a-fA-F]+;") );
            initialized = true;
        }

    };

};

#endif
