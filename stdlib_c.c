#include "stdlib_c.h"

#include <stdlib.h>
#include <string.h> // strlen()
#include <ctype.h> // isprint()

//#define NDEBUG
#include <assert.h>

#ifdef __cplusplus
namespace nl{
#endif

// 現在確保されている領域の個数
static int cnt = 0;
int PRINT_ADDR_FLG = 0;
char BUF_ddGVcooz[1024];

// 文字列を小文字に変換
void tolowers(char *str){ while( (*str = tolower(*str))!='\0' ) str++; }

// mallocのラッパー
// sizeバイトの領域を確保し、先頭アドレスへのポインタを返す。
// エラー時には perror を実行し、exit
// @param size  バイト数
void* new_(size_t size){
  void* ret = malloc(size);
  if(ret == NULL){
	CERRP("malloc(%d)", (int)size);
	perror("malloc in my_stdlib.c");
	exit(EXIT_FAILURE);
  }
  //printf("new    cnt= %d\n",cnt);
  cnt++;

  // 意味のない値をつめる
  {
	size_t i;
	char *ptr = (char*)ret;
	for(i=0;i<size && i<5;i+=2){
	  ptr[i] = 0x5A;
	}
  }
  if(PRINT_ADDR_FLG) printf("malloc addr: %p\n",ret);
  return ret;
}
// reallocのラッパー
// エラー時には perror を実行し、exit
void* renew_(void *ptr,size_t size){
  if(ptr == NULL) return new_(size);
  void* ret = realloc(ptr,size);
  if(size == 0) return NULL;
  if(ret == NULL){
	CERRP("realloc(ptr,%d)", (int)size);
	perror("realloc in my_stdlib.c");
	exit(EXIT_FAILURE);
  }
  return ret;
}

// freeのラッパー
void delete_(void *ptr){
  if(ptr == NULL) return;
  if(PRINT_ADDR_FLG) printf("free   addr: %p\n",ptr);
  free(ptr);
  cnt--;
  //printf("delete cnt= %d\n",cnt);
}

// メモリ開放チェック用
// @return  現在確保されている領域の個数
int getMemCnt(){
  return cnt;
}

#ifdef __cplusplus
}; // namespace nl
#endif
