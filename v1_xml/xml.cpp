// -*- mode: cpp -*-
#include "xml.h"
// created date : 2011/12/07 19:59:43
// last updated : 2012/12/29 21:04:00

#include "util.h"

namespace nl{

  XmlNode::XmlNode()
	: //ns(""),
	name_(new Variable("")), content_(new Variable()), attrs_(){
	nl_INC();
  }
  XmlNode::XmlNode(const std::string &name) :
	//ns(""), 
	name_(new Variable(name)), content_(new Variable()), attrs_(){
	nl_INC();
  }
  XmlNode::XmlNode(XmlScanner &s)
	: //ns(""),
	name_(new Variable("")), content_(new Variable()), attrs_(){
	parse(s);
	nl_INC();
  }

  XmlNode::~XmlNode(){
	nl_DEC();
	//for( List::iterator ite = children_.begin(); ite!=children_.end(); ++ite) (*ite)->parent_  = AbsNameTable::NullPtr;
  }

  XmlNode::XmlNode(const XmlNode &obj) :
	//ns(obj.ns),
	name_(obj.name_), content_(obj.content_), attrs_(obj.attrs_){
	nl_INC();
	copy( &obj );
  }

  XmlNode &XmlNode::operator=(const XmlNode &obj){
	//ns       = obj.ns;
	name_	 = obj.name_;
	content_ = obj.content_;
	attrs_   = obj.attrs_;
	copy(&obj);
	return *this;
  }
  
  XmlNode &XmlNode::attr(const std::string &name, PtrV val){
	attrs_.push_back(Attr(name, val));
	return *this;
  }
  XmlNode &XmlNode::attr(const std::string &name, const std::string &val){
	return attr(name, PtrV(new Variable(val)));
  }
  XmlNode &XmlNode::attr(const std::string &name, const int val){
	char buf[256];
	sprintf(buf, "%d", val);
	return attr(name, buf);
  }

  // 属性追加
  PtrV XmlNode::add(const std::string &name, PtrV var){
	if( find(name) ){
	  DBGP("warning: multiple definition '" << name << "'");
	  return Variable::NullPtr;
	}
	attr(name, var);
	return var;
  }
  PtrV XmlNode::add(const int idx, PtrV ){
	if( find(idx) ){
	  DBGP("warning: multiple definition '" << idx << "'");
	  return Variable::NullPtr;
	}
	return ERRP("cannot add variable with [integer] to Xml '" << idx), Variable::NullPtr;
  }

  // 属性へのポインタを返す
  PtrV XmlNode::find(const std::string &name){
	PtrV ret;
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite)
	  if( ite->first == name ){ ret = ite->second; break; }
	
	if( name == "_content" ){
	  if( ret ) DBGP("warning: reserved word '" << name << "'is used as attr-name");
	  return content();
	}
	if( name == "_name" ){
	  if( ret ) DBGP("warning: reserved word '" << name << "'is used as attr-name");
	  return name_;
	}
	if( name == "this" ){
	  if( ret ) DBGP("warning: reserved word '" << name << "'is used as attr-name");
	  if( !this_ptr() ) ERRP("error: this_ptr is not set");
	  return PtrV(new Variable(this_ptr()));
	}
	if( name == "_parent" ){
	  if( ret ) DBGP("warning: reserved word '" << name << "'is used as attr-name");
	  if( !parent_.lock() ){
		ERRP( "error: parent is not set. " << this->name());
		return PtrV(new Variable(AbsNameTable::NullPtr));
	  }
	  return PtrV(new Variable(parent_.lock()));
	}
	return ret;
  }
  
  // idx番目の 属性へのポインタを返す。無ければ NULL
  PtrV XmlNode::attrAt(int idx){
	if( idx < 0 || (unsigned int)idx >= attrs_.size() ) return PtrV();
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite, idx--){
	  if( idx == 0 ) return ite->second;
	}
	return PtrV();
  }

  // idx番目の 属性の名前を返す。
  std::string XmlNode::attrNameAt(int idx){
	if( idx < 0 || (unsigned int)idx >= attrs_.size() ) return "";
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite, idx--){
	  if( idx == 0 ) return ite->first;
	}
	return "";
  }



  // 子ノードへのポインタを返す
  PtrV XmlNode::find(const int idx){
	Ptr p = childAt(idx);
	if( !p ) return nl::PtrV();
	return nl::PtrV(new nl::Variable(nl::PtrNT(p)));
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
	ptr->this_ptr(ptr);
	return ptr;
  }
  XmlNode::Ptr XmlNode::createFromText(const std::string &text){
	XmlScanner s("text", text);
	XmlNode::Ptr ptr = XmlNode::Ptr(new XmlNode(s));
	ptr->this_ptr(ptr);
	return ptr;
  }


  XmlNode &XmlNode::parse(XmlScanner &s){
	name_   ->assign(s.getName()   );
	content_->assign(s.getContent());
	attrs_   = s.getAttrList();

	for( s.child(); s.valid(); s.next() ){
	  add( new XmlNode(s) );
	}
	s.parent();
	
	return *this;
  }

  // clone = DeppCopy
  XmlNode::Ptr XmlNode::cloneC() const{
	XmlNode::Ptr ptr = XmlNode::Ptr(new XmlNode(name()));
	ptr->this_ptr(ptr);
	ptr->content_ = content_->clone();
	// attrs
	for( AttrList::const_iterator ite = attrs_.begin(); ite != attrs_.end(); ++ite){
	  ptr->attr( ite->first, ite->second->clone() );
	}
	// children
	for( List::const_iterator ite = children_.begin(); ite != children_.end(); ++ite){
	  ptr->add( (*ite)->cloneC() );
	}
	return ptr;
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
	p.start(name_->refOf_val_str());
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite)
	  p.attr(ite->first, ite->second->asStr());
	p.content(content_->asStr());

	for( List::iterator ite=children_.begin(); ite!=children_.end(); ++ite)
	  (*ite)->write(p);
	
	p.end();
  }

  void XmlNode::save_dbg(const std::string &file_name){
	XmlPrinter p(file_name);
	write_dbg(p);
  }


  void XmlNode::write_dbg(XmlPrinter &p){
	p.start(name_->refOf_val_str());
	for( AttrList::iterator ite=attrs_.begin(); ite!=attrs_.end(); ++ite){
	  if(ite->first == "calcPrefSize" ||
		 ite->first == "setSize" ||
		 ite->first == "min_w" || ite->first == "min_h" ||
		 ite->first == "length" ||
		 ite->first == "bg_color" ||
		 //ite->first == "scale" ||
		 ite->first == "orientation" ||
		 ite->first == "c_id" ||
		 ite->first == "enable" ||
		 ite->first == "" ||
		 ite->first == "" ||
		 ite->first == "place" ) continue;
	  p.attr(ite->first, ite->second->asStr());
	}
	p.content(content_->asStr());

	for( List::iterator ite=children_.begin(); ite!=children_.end(); ++ite)
	  (*ite)->write_dbg(p);
	
	p.end();
  }


  // 確認用表示
  void XmlNode::dump(){
	if(depth_ == 0) DBGP("XmlNode::dump() ------------------");
	char buf[512];
	for(unsigned int i=0;i<depth_;i++) buf[i] = ' ';
	buf[depth_] = '\0';
	std::cout << buf << name_->asStr() << std::endl; // DBGP(content_);
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
  doc.attr("from",nl::Variable::Ptr(new nl::Variable("xml.cpp")));
    doc
	  .add( XmlNode("text").attr("id", "0001").attr("number", 102).content("Hello") )
	.add( XmlNode("text").content("Bye") )
	.add( XmlNode("fruit")
		  .add( XmlNode("name").attr("lang","en").content("Apple") )
		  .add( XmlNode("name").attr("lang","ja").content("りんご") )
		  )
  ;
  doc.dump();
  
  doc.save("TestData/output3.xml");

  cout << "write to 'TestData/output3.xml'" << endl;

  cout << "XmlString:" << endl << doc.toStr() << endl;
}


int main(){
  //test();
  //test_XmlNode_scan();
  test_XmlNode_print();

  nl::dump_alloc_status();
  return 0;
}

#endif

