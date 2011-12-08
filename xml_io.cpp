// -*- mode: cpp -*-
#include "xml_io.h"
// created date : 2011/12/07 19:59:43
// last updated : 2011/12/09 01:53:58


#if 0
// test
// $ make TARGET=xml_io
#include <iostream>
using namespace std;
using nl::AttrList;
using nl::XmlScanner;
using nl::XmlPrinter;

void test_scanner(XmlScanner &s); // 入力
void test_printer(); // 出力

int main(){
  XmlScanner s = XmlScanner("TestData/input.xml");
  test_scanner(s);
  XmlScanner s1 = XmlScanner("dat",
							 "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
							 "<root>"
							 "<message>Hello from String</message>"
							 "<fruit> <name>Orange</name> </fruit>"
							 "</root>"
							 );
  test_scanner(s1);
  test_printer();
  return 0;
}

void test_scanner(XmlScanner &s){
  if(s.getName() != "root"){ cout << "error: ルートノードが root ではありません。" << endl; return; }

  cout << s.getName() << ": '" << s.getContent() << "'" << endl;
  for( s.child(); s.valid(); s.next() ){
	cout << " " << s.getName() << ": '" << s.getContent() << "'" << endl;
	for( s.child(); s.valid(); s.next() ){
	  cout << "  " << s.getName() << ": ";
	  AttrList attrs = s.getAttrList();
	  for(AttrList::iterator ite=attrs.begin(); ite!=attrs.end(); ++ite)
		cout << "(" << ite->first << "=" << ite->second << ") ";
	  cout << "'" << s.getContent() << "'" << endl;
	}
	s.parent();
  }
  s.parent();
}

void test_printer(){
  XmlPrinter p("TestData/output.xml");
  
  p.start("root").attr("from","xml_io.cpp");

  p.start("message").attr("id", "0001").attr("number", 102).content("Hello").end();
  p.start("message").content("Bye").end();

  p.start("fruit");
  p.start("name").attr("lang", "en").content("Apple").end();
  p.start("name").attr("lang", "ja").content("りんご").end();
  p.end(); // fruit
  
  p.end(); // root
}


#endif

