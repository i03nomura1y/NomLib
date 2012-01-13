// -*- mode: cpp -*-
#ifndef __NOMLIB_XML_IO_H__
#define __NOMLIB_XML_IO_H__
// created date : 2011/12/07 19:59:43
// last updated : 2012/01/14 02:41:35
// xml_c.h の c++ 版
// XmlScanner / XmlPrinter
// -lxml2 -lws2_32

#include "xml_c.h"
#include "variable.h"
#include <string>
#include <list>

namespace nl{
  class XmlScanner;
  class XmlPrinter;
  typedef nl::NTEntry Attr; // 名前と値のペア
  //typedef std::pair<std::string, nl::Variable::Ptr> Attr;
  typedef std::list<Attr> AttrList;
  
  
  class XmlScanner{
  public:
	// ファイルから
	explicit XmlScanner(const std::string &file_name_) : file_name(file_name_), s(NULL), depth_(0){
	  if((s = new_XML_Scanner(file_name_.c_str())) == NULL) return;
	}
	// 文字列から
	XmlScanner(const std::string &file_name_, const std::string &content_) : file_name(file_name_), s(NULL), depth_(0){
	  if((s = new_XML_Scanner_fromString(content_.c_str())) == NULL) return;
	}
	~XmlScanner(){ if(s != NULL) delete_XML_Scanner(s); }
  
	// 現在指している要素が有効か？(nullでないか？)
	bool valid() const{ return (s!=NULL && XML_valid(s)); }
	int  depth() const{ return depth_; };// 現在の深さ
	bool isRoot() const{ return (depth_==0); } // 現在ルート要素ノードを指している?

	// return タグ名, 内容
	std::string getName()    const{ return valid() ? std::string((char*)(s->ptr->name)) : ""; }
	std::string getContent() const{ return valid() ? std::string(XML_getContent(s)) : ""; }
	// return (属性attr を持つか?)
	bool has(const std::string &attr) const{ return valid() ? XML_hasAttr(s, attr.c_str()) : false; }

	/// return has(attr) ? attrの値 : def;
	int get        (const std::string &attr, const int def) const{
	  return has(attr) ? XML_getAttrInt(s, attr.c_str()) : def;
	}
	std::string get(const std::string &attr, const std::string &def) const{
	  char buf[1024];
	  return has(attr) ? std::string( XML_getAttr(s, attr.c_str(), buf, 1024) ) : def ;
	}
	bool get       (const std::string &attr, const bool def) const{
	  return has(attr) ? XML_getAttrBool(s, attr.c_str()) : def;
	}
  
	// 要素移動 次, 前, 子, 親
	void next(){ XML_nextElem(s); }
	void prev(){ XML_prevElem(s); }
	void child(){ XML_childElem(s); depth_++; }
	void parent(){ if(depth_>0){ XML_parent(s); depth_--; } }
  
	// 属性のリスト <属性名, 値>
	AttrList getAttrList() const{
	  AttrList ret;
	  if(!valid()) return ret;
	  for(xmlAttrPtr attr = s->ptr->properties; attr; attr=attr->next){
		ret.push_back(Attr(std::string((const char*)attr->name),
						   Variable::Ptr(new Variable(attr->children->content))));
	  }
	  return ret;
	}
	
  private:
	std::string file_name; // 開いているファイル名
	XML_Scanner *s;
	int depth_; // 現在の深さ。ルートノードが0。子に行くほど数字が大きくなる。
  };



  class XmlPrinter{
  public:
	XmlPrinter() : file_name(), p(NULL){
	  p = new_XML_StringPrinter();
	}
	explicit XmlPrinter(const std::string &file_name_) : file_name(file_name_), p(NULL){
	  if((p = new_XML_Printer(file_name_.c_str())) == NULL) return;
	}
	~XmlPrinter(){ if(p != NULL) delete_XML_Printer(p); }
	
	const std::string popXmlString(){
	  std::string ret(XML_popXMLStr(p));
	  XML_resetXMLStr(p);
	  return ret;
	}

	// namespace の設定
	XmlPrinter &setNS(const std::string &ns){ XML_setNS(p,ns.c_str()); return *this; }
	
	// 要素の開始/終了
	XmlPrinter &start(const std::string &elem){ XML_startElem(p,elem.c_str()); return *this; }
	XmlPrinter &end(){ XML_endElem(p); return *this; }
	XmlPrinter &put(const std::string &elem, const AttrList &attrs){
	  start(elem); attr(attrs); end(); return *this;
	}
	// 属性(Attribute)を出力
	XmlPrinter &attr(const std::string &name, const std::string &val){
	  XML_putAttr(p,name.c_str(), val.c_str()); return *this;
	}
	XmlPrinter &attr(const std::string &name, int   val){ XML_putAttrInt  (p,name.c_str(), val); return *this; }
	XmlPrinter &attr(const std::string &name, float val){ XML_putAttrFloat(p,name.c_str(), val); return *this; }
	XmlPrinter &attr(const AttrList &attrs){
	  for(AttrList::const_iterator ite=attrs.begin(); ite!=attrs.end(); ++ite)
		attr(ite->first, ite->second->asStr());
	  return *this;
	}
	// 中身を出力
	XmlPrinter &content(const std::string &content){ XML_putStr(p,content.c_str()); return *this; }
  
  private:
	std::string file_name; // 開いているファイル名
	XML_Printer *p;
  };

};

#endif

