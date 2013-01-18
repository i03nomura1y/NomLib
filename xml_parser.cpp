// created date : 2012/12/30 18:13:26
// last updated : 2013/01/18 22:16:11
// 
#include "xml_parser.h"

namespace nl{
    
    void XmlParser::init_rules(){
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
