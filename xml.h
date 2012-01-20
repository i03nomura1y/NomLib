// -*- mode: cpp -*-
#ifndef __NOMLIB_XML_H__
#define __NOMLIB_XML_H__
// created date : 2011/12/07 19:59:43
// last updated : 2012/01/21 00:32:37
// xml_c.h の c++ 版
//  XmlNode : Xml のひとつのタグ(node)を表す
// -lxml2 -lws2_32

#include <string>
#include <list>

#include "xml_io.h"

#include "util.h"
#include "tree_base.h"

namespace nl{
  class XmlNode;
  // std::list<XmlNode> XmlNode::List;
  
  // Xml のひとつのタグを表す。
  class XmlNode : public AbsTree<XmlNode>{
  public: // alias
	typedef hasPtr<XmlNode>::Ptr Ptr;
	// typedef std::list<XmlNode::Ptr> List;
  public: // method
	XmlNode();
	explicit XmlNode(const std::string &name); // タグ名を指定して初期化
	explicit XmlNode(XmlScanner &s); // パース
	~XmlNode();
	XmlNode(const XmlNode &obj);
	XmlNode &operator=(const XmlNode &obj);

	// 属性追加
	XmlNode &attr(const std::string &name, nl::Variable::Ptr val);
	XmlNode &attr(const std::string &name, const std::string &val);
	XmlNode &attr(const std::string &name, const int val);
	// content 
	XmlNode &content(const std::string &con){ content_->assign(con); return *this; }

	/// 子ノード追加
	XmlNode &add(const XmlNode &node){ AbsTree<XmlNode>::add(node); return *this; }
	XmlNode &add(      XmlNode *node){ AbsTree<XmlNode>::add(node); return *this; }
	XmlNode &add(      Ptr      node){ AbsTree<XmlNode>::add(node); return *this; }

	// getter
	const std::string &name() const{ return name_->refOf_val_str(); } // タグ名
	nl::Variable::Ptr content(){ return content_; }

	/// implement NameTable
	Variable::Ptr add(const std::string &name, Variable::Ptr var);
	Variable::Ptr add(const int idx, Variable::Ptr var);
	nl::Variable::Ptr find(const std::string &name); // 属性へのポインタを返す。無ければ NULL
	nl::Variable::Ptr find(const int idx); // idx 番目の属性へのポインタを返す。無ければ NULL
	// clone = DeppCopy
	AbsNameTable::Ptr clone() const{ return cloneC(); }
	XmlNode::Ptr      cloneC() const; // concrete

	//
	int size() const{ return attrs_.size(); } // 登録されている属性の個数

	// パース
	XmlNode &parse(const std::string &file_name);	// ファイル内容をパース このオブジェクトをルートノードにする
	XmlNode &parseText(const std::string &text); // 文字列をパース
	static XmlNode::Ptr create(const std::string &file_name); // ファイル名から XmlNode を生成
	static XmlNode::Ptr createFromText(const std::string &text); // XML文字列から XmlNode を生成
	
	// 文字列に変換
	void save(const std::string &file_name); // ファイルに書き出し
	std::string toStr(); // XML文字列に変換
	
	// 表示
	void dump();	// 確認用表示

  private: // method
	//AbsNameTable::WeakPtr parent() const{ return parent_; }
	//void parent(AbsNameTable::WeakPtr p){ parent_ = p; }
	XmlNode &parse(XmlScanner &s); // XmlScanner から読み込み
	void write(XmlPrinter &p); // XmlPrinter に書き出し

  private: // member
	//std::string ns;      // namespace
	nl::Variable::Ptr name_;    // tag name  string の代わりに Variable::Ptr を使用
	nl::Variable::Ptr content_; //           string の代わりに Variable::Ptr を使用
	AttrList attrs_; // xml_io.h で typedef std::list< pair<string name, nl::Variable::Ptr > >
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

