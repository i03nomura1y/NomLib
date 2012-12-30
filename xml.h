// -*- mode:cpp -*-
#ifndef NL_XML_H
#define NL_XML_H
// created date : 2012/12/30 17:58:09
// last updated : 2012/12/30 21:47:37
// 

// http://www.w3.org/TR/xml/

#include "smart.h"
#include "tree_base2.h"

namespace nl{
    class XmlNode;
    class XmlAttr;
    
    enum{
        XmlNode_None = 0,
        XmlNode_Elem,
        XmlNode_Text,
    };

    // attribute
    class XmlAttr{
    public:
        typedef Smart<XmlAttr>::Ptr Ptr;
        typedef Smart<XmlAttr>::List List;

        std::string name;
        std::string value;
        static Ptr build(){ return Ptr(new XmlAttr()); }
        
    private:
        XmlAttr(){}
    };

    // Node (Element, Text, Comment, ...)
    class XmlNode : public AbsTree2<XmlNode>{
        friend Ptr AbsTree2<XmlNode>::build();
    public:
        typedef Smart<XmlNode>::Ptr Ptr;
        typedef Smart<XmlNode>::List List;
    
    public:
        int type;
        std::string name;
        std::string text;
        XmlAttr::List attr_list;
    public:
        static Ptr build(const std::string &name_){
            Ptr ptr = AbsTree2<XmlNode>::build();
            ptr->type = XmlNode_Elem;
            ptr->name = name_;
            return ptr;
        }
        static Ptr buildText(const std::string &text_){
            Ptr ptr = AbsTree2<XmlNode>::build();
            ptr->type = XmlNode_Text;
            ptr->text = text_;
            return ptr;
        }
        ~XmlNode(){}
    
        void dump();
    private:
        XmlNode() : type(XmlNode_None){}
    };

};

#endif
