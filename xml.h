// -*- mode: cpp -*-
#ifndef __NOMLIB_XML_H__
#define __NOMLIB_XML_H__
// created date : 2011/12/07 19:59:43
// last updated : 2011/12/07 21:25:05
// xml_c.h の c++ 版
// -lxml2 -lws2_32

#include "xml_c.h"
#include <string>
#include <list>

namespace nl{
  class XmlScanner{
  public:
	XmlScanner(const std::string &path) : s(NULL), depth_(0){
	  if((s = new_XML_Scanner(path.c_str())) == NULL) return;
	}
	~XmlScanner(){ if(s != NULL) delete_XML_Scanner(s); }
  
	// 現在指している要素が有効か？(nullでないか？)
	bool valid() const{ return (s!=NULL && XML_valid(s)); }
	int depth() const{ return depth_; };// 現在の深さ
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
	std::list<std::pair<std::string, std::string> > getAttrList() const{
	  std::list<std::pair<std::string, std::string> > ret;
	  if(!valid()) return ret;
	  for(xmlAttrPtr attr = s->ptr->properties; attr; attr=attr->next){
		ret.push_back(std::pair<std::string,std::string>((const char*)attr->name,
														 (const char*)attr->children->content));
	  }
	  return ret;
	}
	
  private:
	XML_Scanner *s;
	int depth_; // 現在の深さ。ルートノードが0。子に行くほど数字が大きくなる。
  };

};

#endif

