#ifndef _NOMLIB_VARSTR_C_H_
#define _NOMLIB_VARSTR_C_H_
// created date : 2011/12/07 18:24:10
// last updated : 2011/12/07 21:25:09
// 可変長文字列
// for C

#ifdef __cplusplus
namespace nl{
#endif

/// variable length String ---------------------------------------
typedef struct varstr_{
  unsigned char *buf;
  unsigned int length;      // 有効なデータ数( = ポインタ )
  unsigned int buf_size; // buf の大きさ
}varstr;

varstr *new_varstr(unsigned int size);
// 新しいインスタンスを生成 & コピー
varstr *clone_varstr(varstr *obj);
void delete_varstr(varstr *this_);
void put_varstr(varstr *this_, unsigned char o);
void putBytes_varstr(varstr *this_,
					 const unsigned char *str,
					 const unsigned int  len);
void putStr_varstr(varstr *this_, const char *str); // 終端 NUL文字
void putVarstr_varstr(varstr *this_, const varstr *str);
unsigned char *popString_varstr(varstr *this_);
void dump_varstr(varstr *this_);

#ifdef __cplusplus
}
#endif


#endif
