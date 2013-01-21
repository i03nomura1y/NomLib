// -*- mode:cpp -*-
#ifndef NL_LISP_H
#define NL_LISP_H
// created date : 2013/01/18 22:46:28
// last updated : 2013/01/21 20:41:43
// 

#include "smart.h"
#include "tree_base.h"

namespace nl{
    class LispNode;
    
    enum{
        LispNode_None = 0,
        LispNode_List,

        LispNode_Comment,
        LispNode_String,
        LispNode_Number,
        LispNode_Literal,
    };

    
    // Node (List, Atom)
    class LispNode : public AbsTree<LispNode>{
        friend Ptr AbsTree<LispNode>::build();
    public:
        typedef Smart<LispNode>::Ptr Ptr;
        typedef Smart<LispNode>::List List;
    
    public:
        int type;
        std::string value;
    public:
        static Ptr build(const int type_, const std::string &value_){
            Ptr ptr = AbsTree<LispNode>::build();
            ptr->type  = type_;
            ptr->value = value_;
            return ptr;
        }
        static Ptr buildList(){
            return build(LispNode_List, "");
        }
        ~LispNode(){}
    
        void dump();
    private:
        LispNode() : type(LispNode_None){}
    };

};

#endif
