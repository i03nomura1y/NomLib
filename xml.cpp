// -*- mode: cpp -*-
// created date : 2012/12/30 17:57:58
// last updated : 2013/01/18 22:19:16
// 

#include "xml.h"
#include <iostream>

namespace nl{
    void XmlNode::dump(){
        if( type == XmlNode_Text ){
            std::cout << " [" << text << "]";
            return;
        }

        std::cout << std::endl;
        for(unsigned int i=0;i<depth_;i++) std::cout << " ";
        std::cout << "(" << name;
        // Attribute
        std::cout << " (";
        for(XmlAttr::List::iterator ite = attr_list.begin(); ite != attr_list.end(); ++ite)
            std::cout << "('" << (*ite)->name << "' '" << (*ite)->value << "')";
        std::cout << ")";
        // Children
        for(XmlNode::List::iterator ite = m_list.begin(); ite != m_list.end(); ++ite)
            (*ite)->dump();
        std::cout << ")";
        if(depth_==0) std::cout << std::endl;
    }
};
