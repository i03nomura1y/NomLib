// -*- mode: cpp -*-
#include "xml.h"
// created date : 2011/12/07 19:59:43
// last updated : 2012/01/13 03:24:55

#include "util.h"

namespace nl{

  XmlNode::XmlNode()
	: //ns(""),
	name_(""), content_(new Variable()), attrs_(), children_(), parent_(NULL), depth_(0), this_ptr(){
	nl_INC();
  }
  XmlNode::XmlNode(const std::string &name) :
	//ns(""), 
	name_(name), content_(new Variable()), attrs_(), children_(), parent_(NULL), depth_(0), this_ptr(){
	nl_INC();
  }
  XmlNode::XmlNode(XmlScanner &s)
	: //ns(""),
	name_(""), content_(new Variable()), attrs_(), children_(), parent_(NULL), depth_(0), this_ptr(){
	parse(s);
	nl_INC();
  }

  XmlNode::~XmlNode(){
	nl_DEC();
	for( List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
	  (*ite)->parent_ = NULL;
  }

  XmlNode::XmlNode(const XmlNode &obj) :
	//ns(obj.ns),
	name_(obj.name_), content_(obj.content_),
	attrs_(obj.attrs_), children_(obj.children_), parent_(obj.parent_),
	depth_(obj.depth_){
	nl_INC();
  }

  XmlNode &XmlNode::operator=(const XmlNode &obj){
	//ns       = obj.ns;
	name_	 = obj.name_;
	content_ = obj.content_;
	attrs_   = obj.attrs_;
	children_ = obj.children_;
	parent_  = obj.parent_;
	depth_   = obj.depth_;
	return *this;
  }
  
  XmlNode &XmlNode::attr(const std::string &name, Variable::Ptr val){
	attrs_.push_back(Attr(name, val));
	return *this;
  }
  XmlNode &XmlNode::attr(const std::string &name, const std::string &val){
	return attr(name, Variable::Ptr(new Variable(val)));
  }
  XmlNode &XmlNode::attr(const std::string &name, const int val){
	char buf[256];
	sprintf(buf, "%d", val);
	return attr(name, buf);
  }

  XmlNode &XmlNode::add(const XmlNode &node){
	XmlNode::Ptr n = XmlNode::Ptr( new XmlNode(node) );
	n->setThisPtr(n);
	return add(n);
  }
  XmlNode &XmlNode::add(XmlNode::Ptr node){
	node->parent_ = this;
	node->updateDepth(depth_+1);
	children_.push_back(node);
	return *this;
  }
  
  Variable::Ptr XmlNode::add(const std::string &name, Variable::Ptr var){
	if( find(name) ){
	  DBGP("warning: multiple definition '" << name << "'");
	  return Variable::NullPtr;
	}
	attr(name, var);
	return var;
  }
  Variable::Ptr XmlNode::add(const int idx, Variable::Ptr ){
	if( find(idx) ){
	  DBGP("warning: multiple definition '" << idx << "'");
	  return Variable::NullPtr;
	}
	return ERRP("cannot add variable to Xml '" << idx), Variable::NullPtr;
  }

  // 属性へのポインタを返す
  Variable::Ptr XmlNode::find(const std::string &name){
	Variable::Ptr ret;
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite)
	  if( ite->first == name ){ ret = ite->second; break; }
	
	if( name == "_content" ){
	  if( ret ) DBGP("warning: reserved word '" << name << "'is used as attr-name");
	  return content();
	}
	if( name == "_name" ){
	  if( ret ) DBGP("warning: reserved word '" << name << "'is used as attr-name");
	  return Variable::Ptr(new Variable(this->name()));
	}
	if( name == "this" ){
	  if( ret ) DBGP("warning: reserved word '" << name << "'is used as attr-name");
	  if( !this_ptr.lock() ) ERRP("error: this_ptr is not set");
	  return Variable::Ptr(new Variable(this_ptr.lock()));
	}
	return ret;
  }
  
  Variable::Ptr XmlNode::find(const int idx){
	if(idx < 0) return Variable::NullPtr;
	int i = idx;
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite, i--)
	  if( i == 0 ) return ite->second;
	return Variable::NullPtr;
  }


  
  // 深さを更新
  void XmlNode::updateDepth(int newDepth){
	depth_ = newDepth;
	for( List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
	  (*ite)->updateDepth(depth_+1);
  }


  /// parse -----------------------------------------------------------------------------------
  // ファイル内容をパース このオブジェクトをルートノードにする
  XmlNode &XmlNode::parse(const std::string &file_name){
	XmlScanner s(file_name);
	return parse(s);
  }
  // 文字列をパース
  XmlNode &XmlNode::parseText(const std::string &text){
	XmlScanner s("text", text);
	return parse(s);
  }

  XmlNode::Ptr XmlNode::create(const std::string &file_name){
	XmlScanner s(file_name);
	XmlNode::Ptr ptr = XmlNode::Ptr(new XmlNode(s));
	ptr->setThisPtr(ptr);
	return ptr;
  }
  XmlNode::Ptr XmlNode::createFromText(const std::string &text){
	XmlScanner s("text", text);
	XmlNode::Ptr ptr = XmlNode::Ptr(new XmlNode(s));
	ptr->setThisPtr(ptr);
	return ptr;
  }


  XmlNode &XmlNode::parse(XmlScanner &s){
	name_    = s.getName();
	content_->assign(s.getContent());
	attrs_   = s.getAttrList();

	for( s.child(); s.valid(); s.next() ){
	  add( XmlNode(s) );
	}
	s.parent();
	
	return *this;
  }

  /// to string ------------------------------------------------------------------------------
  // ファイルに書き出し
  void XmlNode::save(const std::string &file_name){
	XmlPrinter p(file_name);
	write(p);
  }
  // XML文字列に変換
  std::string XmlNode::toStr(){
	XmlPrinter p;
	write(p);
	return p.popXmlString();
  }

  void XmlNode::write(XmlPrinter &p){
	p.start(name_);
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite)
	  p.attr(ite->first, ite->second->asStr());
	p.content(content_->asStr());

	for( List::iterator ite=children_.begin(); ite!=children_.end(); ++ite)
	  (*ite)->write(p);
	
	p.end();
  }


  // 確認用表示
  void XmlNode::dump(){
	if(depth_ == 0) DBGP("XmlNode::dump() ------------------");
	char buf[512];
	for(int i=0;i<depth_;i++) buf[i] = ' ';
	buf[depth_] = '\0';
	std::cout << buf << name_ << std::endl; // DBGP(content_);
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite)
	  std:: cout << buf << "(" << ite->first << " = " << ite->second->asStr() << ")" << std::endl;
	for( List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
	  (*ite)->dump();
  }

};


#ifdef TEST
// test
// $ make TEST=xml
#include <iostream>
using namespace std;
using nl::XmlNode;

void test();
void test_XmlNode_scan();
void test_XmlNode_print();

int main(){
  test();
  test_XmlNode_scan();
  test_XmlNode_print();

  nl::dump_alloc_status();
  return 0;
}

void test(){
  XmlNode::Ptr doc = XmlNode::create("TestData/simple.xml"); // ファイルから
  doc->dump();
}

void test_XmlNode_scan(){
  XmlNode::Ptr doc = XmlNode::create("TestData/input.xml"); // ファイルから
  doc->dump();
  cout << " ---------------- ---------------------- ---------------------- " << endl;
  doc->parseText("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				 "<root>\n"
				 "<message>Hello from String</message>"
				 "<fruit> <name>Orange</name> </fruit>"
				 "</root>\n");
  cout << doc->toStr() << endl;
}

void test_XmlNode_print(){
  cout << " ---------------- ---------------------- ---------------------- " << endl;

  XmlNode doc("rootNode");
  doc.attr("from","xml.cpp");
  doc
	.add( XmlNode("text").attr("id", "0001").attr("number", 102).content("Hello") )
	.add( XmlNode("text").content("Bye") )
	.add( XmlNode("fruit")
		  .add( XmlNode("name").attr("lang","en").content("Apple") )
		  .add( XmlNode("name").attr("lang","ja").content("りんご") )
		  );
  doc.dump();
  
  doc.save("TestData/output3.xml");

  cout << "write to 'TestData/output3.xml'" << endl;

  cout << "XmlString:" << endl << doc.toStr() << endl;
}


#endif

