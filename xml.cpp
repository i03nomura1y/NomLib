// -*- mode: cpp -*-
#include "xml.h"
// created date : 2011/12/07 19:59:43
// last updated : 2012/01/10 00:40:41

#include "util.h"

namespace nl{

  XmlNode::XmlNode()
	: //ns(""),
	name_(""), content_(new Variable()), attrs_(), children_(), parent_(NULL), depth_(0){
  }
  XmlNode::XmlNode(const std::string &name) :
	//ns(""), 
	name_(name), content_(new Variable()), attrs_(), children_(), parent_(NULL), depth_(0){
  }
  XmlNode::XmlNode(XmlScanner &s)
	: //ns(""),
	name_(""), content_(new Variable()), attrs_(), children_(), parent_(NULL), depth_(0){
	parse(s);
  }

  XmlNode::~XmlNode(){
  }

  XmlNode::XmlNode(const XmlNode &obj) :
	//ns(obj.ns),
	name_(obj.name_), content_(obj.content_),
	attrs_(obj.attrs_), children_(obj.children_), parent_(obj.parent_),
	depth_(obj.depth_){
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
  
  XmlNode &XmlNode::attr(const std::string &name, const std::string &val){
	attrs_.push_back(Attr(name, nl::Variable::Ptr(new nl::Variable(val))));
	return *this;
  }
  XmlNode &XmlNode::attr(const std::string &name, const int val){
	char buf[256];
	sprintf(buf, "%d", val);
	attr(name, buf);
	return *this;
  }
  XmlNode &XmlNode::add(XmlNode &node){
	node.parent_ = this;
	node.updateDepth(depth_+1);
	children_.push_back(node);
	return *this;
  }
  
  // 属性へのポインタを返す
  nl::Variable::Ptr XmlNode::find(const std::string &name){
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite)
	  if( ite->first == name ) return ite->second;
	return nl::Variable::NullPtr;
  }
  nl::Variable::Ptr XmlNode::find(unsigned int idx){
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite, idx--)
	  if( idx == 0 ) return ite->second;
	return nl::Variable::NullPtr;
  }


  // 深さを更新
  void XmlNode::updateDepth(int newDepth){
	depth_ = newDepth;
	for( List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
	  ite->updateDepth(depth_+1);
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

  XmlNode XmlNode::create(const std::string &file_name){
	XmlScanner s(file_name);
	return XmlNode(s);
  }
  XmlNode XmlNode::createFromText(const std::string &text){
	XmlScanner s("text", text);
	return XmlNode(s);
  }


  XmlNode &XmlNode::parse(XmlScanner &s){
	name_    = s.getName();
	content_->assign(s.getContent());
	attrs_   = s.getAttrList();

	for( s.child(); s.valid(); s.next() ){
	  XmlNode n(s);
	  add( n );
	}
	s.parent();
	
	return *this;
  }

  /// to string ------------------------------------------------------------------------------
  // ファイルに書き出し
  void XmlNode::writeToFile(const std::string &file_name){
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
	  ite->write(p);
	
	p.end();
  }


  // 確認用表示
  void XmlNode::dump(){
	if(depth_ == 0) DBGP("nl::XmlNode::dump() ------------------");
	char buf[512];
	for(int i=0;i<depth_;i++) buf[i] = ' ';
	buf[depth_] = '\0';
	std::cout << buf << name_ << std::endl; // DBGP(content_);
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite)
	  std:: cout << buf << "(" << ite->first << " = " << ite->second->asStr() << ")" << std::endl;
	for( List::iterator ite = children_.begin(); ite!=children_.end(); ++ite)
	  ite->dump();
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
  //test();
  test_XmlNode_scan();
  //test_XmlNode_print();
  return 0;
}

void test(){
  XmlNode doc = XmlNode::create("TestData/simple.xml"); // ファイルから
  doc.dump();
}

void test_XmlNode_scan(){
  XmlNode doc = XmlNode::create("TestData/input.xml"); // ファイルから
  doc.dump();
  cout << " ---------------- ---------------------- ---------------------- " << endl;
  doc.parseText("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				"<root>\n"
				"<message>Hello from String</message>"
				"<fruit> <name>Orange</name> </fruit>"
				"</root>\n");
  cout << doc.toStr() << endl;
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
  
  doc.writeToFile("TestData/output3.xml");

  cout << "write to 'TestData/output3.xml'" << endl;

  cout << "XmlString:" << endl << doc.toStr() << endl;
}


#endif

