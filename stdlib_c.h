#ifndef _NOMLIB_STD_LIB_C_H_
#define _NOMLIB_STD_LIB_C_H_
// created date : 2011/12/07 18:24:10
// last updated : 2011/12/08 02:24:22
// メモリ管理(malloc,freeなど)関連
// 共通ヘッダ

#include <stdio.h>

#ifdef __cplusplus
namespace nl{
#endif

#ifndef FALSE
#  define FALSE 0
#endif

extern int PRINT_ADDR_FLG;
extern char BUF_ddGVcooz[];

// print debug-message
#define CDBGP(...) CERRP(__VA_ARGS__)
// print error/warning-message
#define CERRP(...) (sprintf(BUF_ddGVcooz,__VA_ARGS__),fprintf(stderr, "%s:%d: in %s(): %s\n", __FILE__, __LINE__, __FUNCTION__, BUF_ddGVcooz))

// 文字列を小文字に変換
void tolowers(char *str);

/// Memory ----------------------------------------------
// mallocのラッパー
// sizeバイトの領域を確保し、先頭アドレスへのポインタを返す。
// エラー時には perror を実行し、exit
// @param size  バイト数
void* new_(size_t size);
// reallocのラッパー
// エラー時には perror を実行し、exit
void* renew_(void* ptr,size_t size);
// freeのラッパー
void delete_(void *ptr);

// メモリ開放チェック用
// @return  現在確保されている領域の個数
int getMemCnt();

#ifdef __cplusplus
}; // namespace nl
#endif

#endif


