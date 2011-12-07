#ifndef _NOMLIB_XML_C_H_
#define _NOMLIB_XML_C_H_
// created date : 2011/12/07 18:24:10
// last updated : 2011/12/08 01:42:49
// XML 読み書きライブラリ (libxml2 のラッパ)
// utf-8 のみ。
// -lxml2 -lws2_32

#include <libxml/xmlwriter.h>
#include <libxml/parser.h>

// convert_charset を使う?
#define USE_CONVERT_CHARSET 0

#ifdef __cplusplus
namespace nl{
#endif

typedef struct{
  xmlDocPtr doc;
  xmlNodePtr parent; // 親を指す(ptrにNULLを許すため)
  xmlNodePtr ptr;
}XML_Scanner;

typedef struct{
  xmlTextWriterPtr writer;
  char ns[512];
}XML_Printer;

// コンストラクタ
// 失敗時: NULL
XML_Printer *new_XML_Printer(const char *file_name);
// デストラクタ
void delete_XML_Printer(XML_Printer *this_);

#if USE_CONVERT_CHARSET
// 読み出し時に SHIFT-JIS に変換するかどうか
void XML_setSJISflag(int flag);
#endif

// namespace の設定
void XML_setNS(XML_Printer *p, const char *ns);

// 要素の開始
void XML_startElem(XML_Printer *p, const char *elem);
// 要素の終了
void XML_endElem(XML_Printer *p);
// 属性(Attribute)を出力
void XML_putAttr     (XML_Printer *p, const char *name,const char *val);
void XML_putAttrInt  (XML_Printer *p, const char *name,const int   val); // 10進文字列
void XML_putAttrHex  (XML_Printer *p, const char *name,const int   val); // 16進文字列
void XML_putAttrFloat(XML_Printer *p, const char *name,const float val);
// 文字列を出力
void XML_putStr(XML_Printer *p, const char *content);
// "%02x "の繰り返しで文字列を出力
// @param length   content の長さ
void XML_putHexStr(XML_Printer *p, const char *content, const int length);

/// 要素の開始
// 要素名, [型名, 属性名, 値,]* NULL
// (p, "Human",  "age", "d", 22,  "name", "s", "Taro",   NULL )
void XML_startElemv(XML_Printer *p, const char *elem, ...);
/// 要素を出力
void XML_putElemv(XML_Printer *p, const char *elem, ...);



/// Getter系 ----
// コンストラクタ
// 失敗時: NULL
XML_Scanner *new_XML_Scanner(const char *file_name);
XML_Scanner *new_XML_Scanner_fromString(const char *text);
// デストラクタ
void delete_XML_Scanner(XML_Scanner *this_);
// 要素名が等しいか？
int XML_isName(XML_Scanner *this_, const char *name);

/// ポインタ移動
// 子の要素を指す
// 子が NULL なら false を返す(ptr は NULL になる)
int XML_childElem(XML_Scanner *this_);
// 親の要素を指す
// 親が NULL なら false を返す(ptr はそのまま)
int XML_parent(XML_Scanner *this_);
// 次/前の要素を指す
void XML_nextElem(XML_Scanner *this_);
void XML_prevElem(XML_Scanner *this_);

// ポインタが有効(NULLでない)なら true
int XML_valid(XML_Scanner *this_);

// 属性を持つか？ nameのアトリビュートを持てば 真 を返す
int XML_hasAttr(XML_Scanner *this_, const char *name);
// xmlNodePtr から属性(Attribute)の値を取り出す ---
// 文字列
// buf が NULL -> 新しく領域を作って返す。 delete する必要あり。
// buf が 非NULL -> buf に文字列をコピーして返す
char *XML_getAttr    (XML_Scanner *this_, const char *name, char *buf,int buf_len);
long  XML_getAttrInt  (XML_Scanner *this_, const char *name); // 10進数 -> long
long  XML_getAttrHex  (XML_Scanner *this_, const char *name); // 16進数 -> long
float XML_getAttrFloat(XML_Scanner *this_, const char *name); // 文字列 -> float
int   XML_getAttrBool (XML_Scanner *this_, const char *name); // 文字列 -> bool
// 内容 戻り値は解放しなくてよい
const char *XML_getContent  (XML_Scanner *this_);

#ifdef __cplusplus
}; // namespace nl
#endif

#endif
