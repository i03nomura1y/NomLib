#include "varstr_c.h"

#include <string.h>
#include <ctype.h>
#include "stdlib_c.h"

#ifdef __cplusplus
namespace nl{
#endif

// 一度に追加するバッファサイズ
#define BUFF_SIZE_STEP 256
varstr *new_varstr(unsigned int size){
  if(size == 0) size = BUFF_SIZE_STEP;
  varstr *this_ = (varstr*)new_(sizeof(varstr));
  this_->buf = (unsigned char*)new_(sizeof(unsigned char)*size);
  this_->buf_size = size;
  this_->length = 0;
  return this_;
}
// 新しいインスタンスを生成 & コピー
varstr *clone_varstr(varstr *obj){
  if(obj == NULL) return NULL;
  varstr *this_ = new_varstr( obj->buf_size );
  memcpy( this_->buf, obj->buf, obj->length );
  this_->length = obj->length;
  return this_;
}
void delete_varstr(varstr *this_){
  if(this_ == NULL) return;
  delete_(this_->buf);
  delete_(this_);
}
void put_varstr(varstr *this_, unsigned char o){
  if(this_ == NULL) return;
  if(this_->length >= this_->buf_size){
	this_->buf_size = (this_->length + BUFF_SIZE_STEP);
	this_->buf = (unsigned char*)renew_( this_->buf, sizeof(unsigned char)*this_->buf_size );
  }
  this_->buf[this_->length] = o;
  this_->length++;
}
void putBytes_varstr(varstr *this_,
					 const unsigned char *str,
					 const unsigned int  len){
  if(this_ == NULL || str == NULL) return;
  unsigned int i;
  for(i=0;i<len;i++) put_varstr(this_, str[i]);
}
void putStr_varstr(varstr *this_, const char *str){
  if(this_ == NULL || str == NULL) return;
  putBytes_varstr(this_, (unsigned char*)str, strlen(str));
}
void putVarstr_varstr(varstr *this_, const varstr *str){
  if(this_ == NULL || str == NULL) return;
  putBytes_varstr(this_, str->buf, str->length);
}


unsigned char *popString_varstr(varstr *this_){
  if(this_ == NULL) return NULL;
  put_varstr(this_,'\0');
  unsigned char *ret = this_->buf;
  this_->buf = NULL;
  return ret;
}

void dump_varstr(varstr *this_){
  if(this_ == NULL || this_->buf == NULL) return;
  unsigned int i,j;
  char hex[64],txt[32];
  for(i=0; i<this_->length; ){
	/// 1行 = 16Byte分
	// 16Byte分を hex と txt へ文字列連結
	for(j=0; j<16 && i<this_->length; j++,i++){
	  unsigned char c = this_->buf[i];
	  sprintf(hex+j*3, "%02x ",c);	// hex <- HexStr(c)
	  txt[j] = isprint(c)?c:'.';
	}
	// NUL文字で文字列を閉じる
	hex[j*3] = txt[j] = '\0';
	// 表示
	printf("%-48s %-16s\n",hex,txt);
  }
}

#ifdef __cplusplus
}; // namespace nl
#endif
