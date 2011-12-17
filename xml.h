// -*- mode: cpp -*-
#ifndef __NOMLIB_XML_H__
#define __NOMLIB_XML_H__
// created date : 2011/12/07 19:59:43
// last updated : 2011/12/14 13:55:26
// xml_c.h の c++ 版
//  XmlNode : Xml のひとつのタグ(node)を表す
// -lxml2 -lws2_32

#include <string>
#include <list>

#include "xml_io.h"

namespace nl{
  class XmlNode;
  typedef std::list<XmlNode> NodeList;

  
  // Xml のひとつのタグを表す
  class XmlNode{
  public:
	XmlNode();
	explicit XmlNode(const std::string &name); // タグ名を指定して初期化
	~XmlNode(){};
	XmlNode(const XmlNode &obj);
	XmlNode &operator=(const XmlNode &obj);
	XmlNode &attr(const std::string &name, const std::string &val);
	XmlNode &attr(const std::string &name, const int val);
	XmlNode &add(XmlNode &node);
	XmlNode &content(const std::string &con){ content_ = con; return *this; }
	const std::string &name() const{ return name_; }

	// パース
	XmlNode &parse(const std::string &file_name);	// ファイル内容をパース このオブジェクトをルートノードにする
	XmlNode &parseText(const std::string &text); // 文字列をパース
	static XmlNode create(const std::string &file_name); // ファイル名から XmlNode を生成
	static XmlNode createFromText(const std::string &text); // XML文字列から XmlNode を生成
	
	// 文字列に変換
	void writeToFile(const std::string &file_name); // ファイルに書き出し
	std::string toStr(); // XML文字列に変換
	
	// 表示
	void dump();	// 確認用表示

  private:
	XmlNode *parent() const{ return parent_; }
	void updateDepth(int newDepth);	// 深さを更新
	XmlNode &parse(XmlScanner &s); // XmlScanner から読み込み
	void write(XmlPrinter &p); // XmlPrinter に書き出し

  private:
	//std::string ns;      // namespace
	std::string name_;    // tag name
	std::string content_; //
	AttrList attrs_; // xml_io.h で typedef
	NodeList children; //std::list<XmlNode> children;
	mutable XmlNode *parent_; // 親ノードへのポインタ
	int depth_; // ルートから数えた深さ。ルートは0
  };



// class XmlPtr{
// public:
//   XmlPtr(XmlNode &node) : root_(node), ptr_(&node){};
//   ~XmlPtr(){};
//   XmlPtr(const XmlPtr &obj) : root_(obj.root_), ptr_(obj.ptr_){}
//   XmlPtr &operator=(const XmlPtr &obj){
// 	root_ = obj.root_;
// 	ptr_  = obj.ptr_;
// 	return *this;
//   }
//   
//   XmlNode &root() const{ return root_; }
//   
//   XmlPtr &add(XmlNode &node){ ptr_->add(node); return *this; }
//   XmlPtr &addMove(XmlNode &node){ ptr_->add(node); ptr_ = &node; return *this; }
//   XmlPtr &parent(){ ptr_ = ptr_->parent(); return *this; }
//   
// 
// public:
//   XmlNode &root_;
//   XmlNode *ptr_;
// };



};

#endif

