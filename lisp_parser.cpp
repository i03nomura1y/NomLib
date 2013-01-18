// created date : 2013/01/18 23:07:19
// last updated : 2013/01/19 00:11:51
// 
#include "lisp_parser.h"

namespace nl{
    
    void LispParser::init_rules(){
        static bool initialized = false;
        if(initialized) return;
            
        Blank   = LexRule("(^\\s+)|(^$)");
        Comment = LexRule("^;(.*)$", 1);
        String = LexRule("^\"(((\\\\.)|[^\"])*)\"",1 ); // 文字列
        Number = LexRule("^[-+]?[0-9]+([.][0-9]*)?");
        Literal = LexRule("^[^;()\t\v\f \r\n]+");
        
        initialized = true;
    }

};
