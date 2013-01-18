// -*- mode: cpp -*-
// created date : 2013/01/18 22:51:25
// last updated : 2013/01/19 00:14:01
// 

#include "lisp.h"
#include <iostream>

namespace nl{
    void LispNode::dump(){
        if( type != LispNode_List ){
            std::cout << value << " ";
            return;
        }
        std::cout << "(";
        // Children
        for(LispNode::List::iterator ite = m_list.begin(); ;){
            (*ite)->dump();
            ++ite;
            std::cout << std::endl;
            for(unsigned int i=0;i<depth_;i++) std::cout << " ";
            if(ite == m_list.end()){ break; }
            
        }
        std::cout << ")";
        
        //for(unsigned int i=0;i<depth_;i++) std::cout << " ";
        if(depth_==0) std::cout << std::endl;
    }
};


