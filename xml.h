// -*- mode:cpp -*-
#ifndef NL_XML_H
#define NL_XML_H
// created date : 2012/12/30 17:58:09
// last updated : 2012/12/30 18:09:06
// 

// http://www.w3.org/TR/xml/

#include <iostream>
#include "smart.h"

namespace nl{

    class XmlNode_I;  // ノードのインタフェース。ツリー情報を持つ
    class XmlElement; // extends XmlNode_I
    class XmlAttr;
    

    class XmlNode_I{
    public:
        typedef nl::Smart<XmlNode_I>::Ptr Ptr;
    public:
        virtual ~XmlNode_I(){}
        virtual void dump(){}
        virtual void updateDepth(unsigned int newDepth){ depth_ = newDepth; }
    protected:
        // ツリー情報
        unsigned int depth_;
        nl::Smart<XmlNode_I>::WeakPtr this_ptr_;
    public:
        nl::Smart<XmlNode_I>::WeakPtr parent_;
    protected:
        XmlNode_I() : depth_(0){}
    };

    // CharData
    class XCharData : public XmlNode_I{
    public:
        typedef nl::Smart<XCharData>::Ptr Ptr;
    public:
        std::string data;
        void dump(){
            std::cout << " [" << data << "]";
        }
        static Ptr build(const std::string &data_){
            Ptr ptr(new XCharData(data_));
            ptr->this_ptr_ = ptr;
            return ptr;
        }
    private:
        XCharData(const std::string &data_) : data(data_){}
    };



    // attribute
    class XmlAttr{
    public:
        typedef nl::Smart<XmlAttr>::Ptr Ptr;

        std::string name;
        std::string value;
        static Ptr build(){ return Ptr(new XmlAttr()); }
    private:
        XmlAttr(){}
    };


    class XmlElement : public XmlNode_I{
    public:
        typedef nl::Smart<XmlElement>::Ptr Ptr;
    
    public:
        // このエレメントの情報
        std::string name;
        nl::Smart<XmlAttr>::List attr_list;
    private:
        // 子エレメント
        nl::Smart<XmlNode_I>::List children;
    
    public:
        ~XmlElement(){}
    
        void add(const XmlNode_I::Ptr &obj){
            if( !obj ) return;
            children.push_back(obj);
            obj->updateDepth(depth_+1);
            obj->parent_ = this_ptr_;
        }
    
        void updateDepth(unsigned int newDepth){
            XmlNode_I::updateDepth(newDepth);
            for(nl::Smart<XmlNode_I>::List::iterator ite = children.begin(); ite != children.end(); ++ite)
                (*ite)->updateDepth(depth_+1);
        }

        void dump(){
            std::cout << std::endl;
            for(unsigned int i=0;i<depth_;i++) std::cout << " ";
            std::cout << "(" << name;
            // Attribute
            std::cout << " (";
            for(nl::Smart<XmlAttr>::List::iterator ite = attr_list.begin(); ite != attr_list.end(); ++ite)
                std::cout << "(" << (*ite)->name << " " << (*ite)->value << ")";
            std::cout << ")";
            // Children
            for(nl::Smart<XmlNode_I>::List::iterator ite = children.begin(); ite != children.end(); ++ite)
                (*ite)->dump();
            std::cout << ")";
            if(depth_==0) std::cout << std::endl;
        }
    
        static XmlElement::Ptr build(const std::string &name_){
            Ptr ptr(new XmlElement());
            ptr->name      = name_;
            ptr->this_ptr_ = ptr;
            return ptr;
        }
    private:
        XmlElement(){}
    };

};

#endif
