// created date : 2011/12/07 18:24:10
// last updated : 2011/12/10 18:54:21
#include "xml_c.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "stdlib_c.h"

#ifdef __cplusplus
namespace nl{
#endif

#define ENCODING "UTF-8"

// warning/error のときに文字列を出すか?
#define PRINT_WARNING 1

#if USE_CONVERT_CHARSET
#  include "convert_charset.h"
  // 暫定的に
  static int sjis_flag = 0;
  void XML_setSJISflag(int flag){ sjis_flag = flag; }
#endif

// private: 属性リストを追加(可変長引数を解析する)
static void XML_putAttrs_core(XML_Printer *p, va_list *args);

// private: ドキュメント開始部分
static XML_Printer *start_XML_Printer(XML_Printer *p){
  xmlTextWriterSetIndent(p->writer, 1);
  
  if( xmlTextWriterStartDocument(p->writer, NULL, ENCODING, NULL) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
	delete_XML_Printer(p);
	return NULL;
  }
  return p;
}
// private: コンストラクタ 共通部分
static XML_Printer *new_XML_Printer_core(xmlTextWriterPtr w){
  XML_Printer *p = (XML_Printer*)new_(sizeof(XML_Printer));
  strcpy(p->ns,"");
  p->bufp = NULL;
  if( (p->writer = w) == NULL){
#if PRINT_WARNING
	CERRP("error");
#endif
	delete_(p);
	return NULL;
  }
  return start_XML_Printer(p);
}

// コンストラクタ
// 失敗時: NULL
XML_Printer *new_XML_Printer(const char *file_name){
  assert(file_name != NULL);
  return new_XML_Printer_core(xmlNewTextWriterFilename(file_name,0));
}

XML_Printer *new_XML_StringPrinter(){
  xmlBufferPtr ptr = xmlBufferCreate();
  XML_Printer *ret = new_XML_Printer_core(xmlNewTextWriterMemory(ptr, FALSE));
  if(ret == NULL){
	xmlBufferFree(ptr);
	return NULL;
  }
  ret->bufp = ptr;
  return ret;
}
// デストラクタ
void delete_XML_Printer(XML_Printer *this_){
  if(this_ == NULL) return;
  if(this_->writer != NULL) xmlFreeTextWriter(this_->writer);
  if(this_->bufp != NULL) xmlBufferFree(this_->bufp);
  delete_(this_);
}

// 作成されたXML文字列を取り出す
// 戻り値は解放しなくてよい
// StringPrinter として new している場合のみ
const char *XML_popXMLStr(XML_Printer *p){
  assert(p != NULL);
  if(p->bufp == NULL) return NULL;
  //xmlFreeTextWriter(p->writer);
  xmlTextWriterEndDocument(p->writer);
  //p->writer = xmlNewTextWriterMemory(p->bufp, FALSE);
  return (const char*)xmlBufferContent(p->bufp);
}
// 作成されたXML文字列を初期化
void XML_resetXMLStr(XML_Printer *p){
  assert(p != NULL);
  if(p->bufp == NULL) return;
  xmlBufferEmpty(p->bufp); // 文字列を初期化
  start_XML_Printer(p);    // ドキュメントを新しく開始
}


// namespace の設定
void XML_setNS(XML_Printer *p, const char *ns){
  assert(p != NULL);
  if(ns == NULL) ns = "";
  strcpy(p->ns, ns);
}


// 要素の開始
void XML_startElem(XML_Printer *p, const char *elem){
  assert(p != NULL);
  assert(elem != NULL);

  if( xmlTextWriterStartElementNS(p->writer,
								  (p->ns[0] != '\0')?BAD_CAST p->ns:NULL,
								  BAD_CAST elem, NULL) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
  }
}
// 要素の終了
void XML_endElem(XML_Printer *p){
  assert(p != NULL);

  if( xmlTextWriterEndElement(p->writer) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
  }
}
// 属性(Attribute)を出力
void XML_putAttr(XML_Printer *p, const char *name,const char *val){
  assert(p != NULL);
  assert(name != NULL);
  assert(val != NULL);

  char *buf;
#if USE_CONVERT_CHARSET
  if(sjis_flag){
	// ENCODING にShiftJIS が指定できない -> UTF-8に変換して出力
	int buf_len = strlen(val)*3+1; // 元の長さx3 +1
	buf = (char*)new_(buf_len);
	convert_charset( val, buf, buf_len, NULL, "UTF-8");
  }
  else
#endif
  {
	buf = (char*)new_(strlen(val)+1);
	strcpy(buf, val);
  }
  if( xmlTextWriterWriteAttribute(p->writer, BAD_CAST name, BAD_CAST buf ) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
  }
  delete_(buf);
}
void XML_putAttrInt(XML_Printer *p, const char *name,const int val){
  assert(p != NULL);
  assert(name != NULL);

  if( xmlTextWriterWriteFormatAttribute(p->writer, BAD_CAST name, "%d", val ) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
  }
}
void XML_putAttrHex(XML_Printer *p, const char *name,const int val){
  assert(p != NULL);
  assert(name != NULL);

  if( xmlTextWriterWriteFormatAttribute(p->writer, BAD_CAST name, "%02x", val ) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
  }
}
void XML_putAttrFloat(XML_Printer *p, const char *name,const float val){
  assert(p != NULL);
  assert(name != NULL);

  char buf[256];
  int i;
  sprintf(buf,"%-200.64f",val);
  // 文字列右側の 空白 と 0 を消す
  for(i=0; i<256 && buf[i]!=' ' && buf[i]!='\0'; i++);
  for(;i>=0 && (buf[i] == '0' || buf[i] == ' ');i--) buf[i] = '\0';
  // 小数点以下に何もない場合は 0 を追加
  if(buf[i] == '.') buf[i+1] = '0';

  //if( xmlTextWriterWriteFormatAttribute(p->writer, BAD_CAST name, "%f", val ) < 0){
  if( xmlTextWriterWriteAttribute(p->writer, BAD_CAST name, BAD_CAST buf ) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
  }
}
// 文字列を出力
void XML_putStr(XML_Printer *p, const char *content){
  assert(p != NULL);
  assert(content != NULL);

  if( xmlTextWriterWriteString(p->writer, BAD_CAST content) < 0){
#if PRINT_WARNING
	CERRP("error");
#endif
  }
}
// "%02x "の繰り返しで文字列を出力
// @param length   content の長さ
void XML_putHexStr(XML_Printer *p, const char *content, const int length){
  int i;
  char buf[8];
  for(i=0;i<length;i++){
	sprintf(buf,"%02x ",(unsigned char)content[i]);
	XML_putStr(p,buf);
  }
}

/// XML タグ開始
// 要素名, [型名, 属性名, 値,]* NULL
// (p, "Human", "d", "age", 22, "s", "name", "Taro", NULL )
void XML_startElemv(XML_Printer *p, const char *elem, ...){
  va_list args;
  va_start(args, elem);
  XML_startElem(p,elem);
  XML_putAttrs_core(p,&args);
  va_end(args);
}
void XML_putElemv(XML_Printer *p, const char *elem, ...){
  va_list args;
  va_start(args, elem);
  XML_startElem(p, elem);
  XML_putAttrs_core(p,&args);
  XML_endElem(p);
  va_end(args);
}

// コンストラクタ
// 失敗時: NULL
static XML_Scanner *new_XML_Scanner_core(xmlDocPtr doc){
  XML_Scanner *this_ =(XML_Scanner*)new_(sizeof(XML_Scanner));
  this_->ptr = this_->parent = NULL;
  if((this_->doc = doc) == NULL) goto NEW_XMLSC_ERR;
  if((this_->ptr = xmlDocGetRootElement(this_->doc)) == NULL) goto NEW_XMLSC_ERR;
  return this_;
 NEW_XMLSC_ERR:
  delete_XML_Scanner(this_);
  return NULL;
}

XML_Scanner *new_XML_Scanner(const char *file_name){
  return new_XML_Scanner_core(xmlParseFile(file_name));
}
XML_Scanner *new_XML_Scanner_fromString(const char *text){
  return new_XML_Scanner_core(xmlParseMemory(text, strlen(text)));
}

// デストラクタ
void delete_XML_Scanner(XML_Scanner *this_){
  if(this_==NULL) return;
  if(this_->doc != NULL) xmlFreeDoc(this_->doc);
  delete_(this_);
}
// 要素名が等しいか？
int XML_isName(XML_Scanner *this_, const char *name){
  return (xmlStrcmp(this_->ptr->name, BAD_CAST name) == 0);
}
// 子の要素を指す
// 子が NULL なら false を返す(ptr は NULL になる)
int XML_childElem(XML_Scanner *this_){
  if(this_->ptr == NULL) return 0;
  this_->parent = this_->ptr;
  this_->ptr = this_->ptr->children;
  for(; this_->ptr && this_->ptr->type != XML_ELEMENT_NODE; this_->ptr = this_->ptr->next);
  return (this_->ptr != NULL);
}
// 親の要素を指す
// 親が NULL なら false を返す(ptr はそのまま)
int XML_parent(XML_Scanner *this_){
  if(this_->parent == NULL) return 0;
  this_->ptr = this_->parent;
  this_->parent = this_->ptr->parent;
  return 1;
}
// 次の要素を指す
void XML_nextElem(XML_Scanner *this_){
  while( (this_->ptr = this_->ptr->next)!=NULL && this_->ptr->type != XML_ELEMENT_NODE);
}
void XML_prevElem(XML_Scanner *this_){
  while( (this_->ptr = this_->ptr->prev)!=NULL && this_->ptr->type != XML_ELEMENT_NODE);
}

// ポインタが有効(NULLでない)なら true
int XML_valid(XML_Scanner *this_){
  return (this_->ptr != NULL);
}

// 属性を持つか？ nameのアトリビュートを持てば 真 を返す
int XML_hasAttr(XML_Scanner *this_, const char *name){
  return (xmlHasProp(this_->ptr,BAD_CAST name) != NULL);
}
// 文字列を返す
char  *XML_getAttr    (XML_Scanner *this_, const char *name, char *buf,int buf_len){
  assert(this_ != NULL && name!=NULL);
  if(buf != NULL) buf[0] = '\0';  // 初期化
  char *tmp = NULL;
  {
	xmlChar *prop = xmlGetProp(this_->ptr,BAD_CAST name);
	if(prop != NULL){
#if USE_CONVERT_CHARSET
	  if(sjis_flag){
		// propの文字コードを SHIFT-JIS に変換
		int tmp_len = strlen((char*)prop)*3+1; // 元の長さx3 +1
		tmp = (char*)new_(tmp_len);
		convert_charset( (char*)prop, tmp, tmp_len, NULL, "SHIFT-JIS");
	  }
	  else
#endif
	  {
		tmp = (char*)new_(strlen((char*)prop)+1);
		strcpy(tmp, (char*)prop);
	  }
	  xmlFree(prop);
	}
  }
  
  // buf が NULL -> new する
  if(buf == NULL){
	buf_len = ( (tmp!=NULL) ? strlen(tmp) : 0 ) + 1;
	buf = (char*)new_(buf_len);
  }
  strncpy(buf, (tmp!=NULL) ? tmp : "" , buf_len-1);
  buf[buf_len-1] = '\0';
  delete_(tmp);
  return buf;
}
// 10進数 -> long
long  XML_getAttrInt  (XML_Scanner *this_, const char *name){
  xmlChar *prop = xmlGetProp(this_->ptr,BAD_CAST name);
  if(prop == NULL) return 0;
  long ret = strtol((char*)prop,NULL,10);
  xmlFree(prop);
  return ret;
}
 // 16進数 -> long
long  XML_getAttrHex  (XML_Scanner *this_, const char *name){
  xmlChar *prop = xmlGetProp(this_->ptr,BAD_CAST name);
  if(prop == NULL) return 0;
  long ret = strtol((char*)prop,NULL,16);
  xmlFree(prop);
  return ret;
}
 // 文字列 -> float
float XML_getAttrFloat(XML_Scanner *this_, const char *name){
  xmlChar *prop = xmlGetProp(this_->ptr,BAD_CAST name);
  if(prop == NULL) return 0.0F;
  float ret = (float)strtod((const char*)prop,NULL);
  xmlFree(prop);
  return ret;
}
// 文字列 -> bool
int XML_getAttrBool(XML_Scanner *this_, const char *name){
  xmlChar *prop = xmlGetProp(this_->ptr,BAD_CAST name);
  if(prop == NULL) return FALSE;
  int len = strlen((char*)prop);
  // 小文字にする
  tolowers((char*)prop);
  // bool として比較
  int ret = ((strcmp((char*)prop,"true")==0) || (len==1 && prop[0]=='1'));
  xmlFree(prop);
  return ret;
}

// 内容 戻り値は解放しなくてよい
const char *XML_getContent  (XML_Scanner *this_){
  return (this_->ptr->children == NULL ||
		  this_->ptr->children->content == NULL)
	? "" : (char*)(this_->ptr->children->content);
}


// 属性リストを追加(可変長引数を解析する)
static void XML_putAttrs_core(XML_Printer *p, va_list *args){
  char *type, *name;
  while((type = va_arg(*args, char*)) != NULL &&
		(name = va_arg(*args, char*)) != NULL ){
	switch(type[0]){
	case 'i': /* FALL-THROUGH*/
	case 'd': XML_putAttrInt  (p, name, va_arg(*args, int)   ); break;
	case 'f': XML_putAttrFloat(p, name, va_arg(*args, double) ); break;
	case 's':
	default:  XML_putAttr     (p, name, va_arg(*args, char*) ); break;
	}
  }
}

#ifdef __cplusplus
}; // namespace nl
#endif



#ifdef TEST
// test
// $ make TEST=xml_c
// $ gcc -o xml_c xml_c.c stdlib_c.c -DTEST -I/usr/include/libxml2 -lxml2 -Wall -Wextra
#include <stdio.h>

#ifdef __cplusplus
using namespace nl;
#endif

void test_scanner(XML_Scanner *s); // 入力
void test_printer(const char *file_name); // 出力: 文字列ならファイルへ、NULLなら文字列化

int main(){
  /*
  test_scanner(new_XML_Scanner("TestData/input.xml"));
  test_scanner(new_XML_Scanner_fromString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
										  "<root>"
										  "<message>Hello from String</message>"
										  "<fruit> <name>Orange</name> </fruit>"
										  "</root>"
										  ));
  puts("");
  test_printer("TestData/output.xml");
  //*/
  test_printer(NULL);
  return 0;
}

void test_scanner(XML_Scanner *s){
  if(s == NULL){ puts("XML parse error."); return; }
  if(!XML_isName(s,"root")){ puts("error: ルートノードが root ではありません。"); goto END_OF_INPUT;}
  puts("root:");
  
  for(XML_childElem(s); XML_valid(s); XML_nextElem(s)){
	if( XML_isName(s,"message") ){
	  printf(" message: %s\n",XML_getContent(s));
	}else if( XML_isName(s,"fruit") ){
	  puts(" fruit: ");
	  for(XML_childElem(s); XML_valid(s); XML_nextElem(s)){
		if( XML_isName(s,"name") ){
		  char buf[256];
		  buf[0] = '\0';
		  if( XML_hasAttr(s,"lang") ) XML_getAttr(s, "lang", buf, 256);
		  printf("  name: (%s) %s\n",buf, XML_getContent(s));
		}
	  }
	  XML_parent(s);
	}
  }
  XML_parent(s);
  
 END_OF_INPUT:
  delete_XML_Scanner(s);
}

void test_printer(const char *file_name){
  XML_Printer *p;
  if(file_name != NULL){
	p = new_XML_Printer(file_name);
  }else{
	p = new_XML_StringPrinter();
  }
  
  XML_startElem(p, "root");

  XML_startElemv(p, "message",
				 "s", "id",     "0001",
				 "d", "number", 102,
				 NULL);
  XML_putStr(p, "Hello");
  XML_endElem(p);
  
  XML_startElem(p, "message");
  XML_putStr(p, "Bye");
  XML_endElem(p);

  XML_startElem(p, "fruit");
  XML_startElemv(p, "name",  "s", "lang", "en", NULL);
  XML_putStr(p, "Apple");
  XML_endElem(p);
  XML_startElemv(p, "name",  "s", "lang", "ja", NULL);
  XML_putStr(p, "りんご");
  XML_endElem(p);
  XML_endElem(p);
  
  XML_endElem(p);

  if(file_name != NULL){
	printf("write To '%s'\n",file_name);
  }else{
	printf("created xml:\n%s\n",XML_popXMLStr(p));
	XML_resetXMLStr(p);
	XML_startElem(p, "message");
	XML_startElem(p, "message");
	XML_putStr(p, "Bye");
	XML_endElem(p);
	XML_endElem(p);
	printf("created xml:\n%s\n",XML_popXMLStr(p));
	
  }
  
  delete_XML_Printer(p);
  
}


#endif

