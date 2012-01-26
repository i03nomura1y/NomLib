#include "image_c.h"

#include <stdio.h>
#include <string.h>
#include "stdlib_c.h"

#ifdef __cplusplus
namespace nl{
#endif

/// private:
// 画像のフォーマットを特定し、幅と高さを取得する。
static void calcImageSize(ImageData *this_);

// 画像ファイルを読み込み、
// type,width,heightを計算
// @param file_name 読み込む画像ファイル名
int open_ImageFile(ImageData *this_,const char *file_name){
  FILE *fp;

  this_->width = -1;
  this_->height = -1;
  this_->type = Type_NONE;

  if((fp = fopen(file_name,"rb")) == NULL){
	fprintf(stderr,
			"error:  %s(%d)::open_ImageFile() - cannot open File '%s'\n",
			__FILE__,__LINE__,file_name);
	return -1;
  }
  // ファイルサイズを求める
  {
	fpos_t fsize;
	// MinGW では __pos が不要
#if __WIN32__
	fsize = 0;
#else
	fsize.__pos = 0;
#endif
	fseek(fp,0,SEEK_END);
	fgetpos(fp,&fsize);
	// MinGW では __pos が不要
#if __WIN32__
	this_->data_size = (unsigned int)fsize;
#else
	this_->data_size = (unsigned int)fsize.__pos;
#endif
	rewind(fp);
  }

  // 領域確保とデータ読み込み
  this_->raw_data = (byte*)new_(this_->data_size);
  {
	int c;
	unsigned int i;
	for(i=0;i<this_->data_size; i++){
	  if(feof(fp) || (c=fgetc(fp))==EOF){
		fprintf(stderr,
				"error:  %s(%d)::open_ImageFile() - can not read File '%s'\n",
				__FILE__,__LINE__,file_name);
		this_->data_size = 0;
		delete_(this_->raw_data);
		fclose(fp);
		this_->raw_data = NULL;
		return -1;
	  }
	  this_->raw_data[i] = (byte)c;
	}
  }
  fclose(fp);

  // 画像のフォーマット特定、サイズの計算
  calcImageSize(this_);
  return 0;
}

/// private: 各ファイル形式毎の画像サイズを計算
// @param data  JPEGデータ
void calcImageSize_JPG(const unsigned char *data,unsigned int len,int *width,int *height){
  unsigned int i;
  for(i=0;i<len-8;i++){
	if(data[0] == 0xFF && data[1]==0xC0){
	  *height = (data[5]<<8)|data[6];
	  *width  = (data[7]<<8)|data[8];
	  return;
	}
	data++;
  }
}

// @param data  PNGデータ
void calcImageSize_PNG(const unsigned char *data,unsigned int len,int *width,int *height){
  unsigned int i,j;
  for(i=0;i<len-12;i++){
	if(strncmp((char*)data,"IHDR",4)==0){
	  *width = *height = 0;
	  for(j=0;j<4;j++){
		*width  = ((*width )<<8) | data[4+j];
		*height = ((*height)<<8) | data[8+j];
	  }
	  return;
	}
	data++;
  }
}
// @param data  GIFデータ
void calcImageSize_GIF(const unsigned char *data,unsigned int len,int *width,int *height){
  if(len < 9) return;
  *width  = (data[7]<<8)|data[6];
  *height = (data[9]<<8)|data[8];
}

// 画像のフォーマットを特定し、幅と高さを取得する。
static void calcImageSize(ImageData *this_){
  // フォーマットをヘッダで判断する
  static const char jpg_hdr[]  = {0xFF, 0xD8, 0}; // 2
  static const char jpg_hdr2[] = {0xFF, 0xD9, 0xFF, 0xD8, 0}; // 4
  static const char png_hdr[]  = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0}; // 8
  static const char gif_hdr[]  = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0}; // 6

  byte *data = this_->raw_data;
  unsigned int len = this_->data_size;
  int width = -1;
  int height = -1;

  if(strncmp((char*)data,jpg_hdr,2) == 0){ // jpg
	calcImageSize_JPG(data, len, &width, &height);
	this_->type = Type_JPEG;
  }else if(strncmp((char*)data,jpg_hdr2,4) == 0){ // jpg
	calcImageSize_JPG(data+4, len-4, &width, &height);
	this_->type = Type_JPEG;
  }else if(strncmp((char*)data,png_hdr,8) == 0){ // png
	calcImageSize_PNG(data, len, &width, &height);
	this_->type = Type_PNG;
  }else if(strncmp((char*)data,gif_hdr,6) == 0){ // gif
	calcImageSize_GIF(data, len, &width, &height);
	this_->type = Type_GIF;
  }else{ // 不定
	this_->type = Type_NONE;
  }
  this_->width = width;
  this_->height = height;
}


// JPEGの 'erroneous header' (0xFF 0xD9 0xFF 0xD8) を先頭に追加
// 参考: SWF V10 仕様書 P.148
void addErroneousHeader_ImageFile(ImageData *this_){
  if(this_->type != Type_JPEG) return;

  unsigned int i;
  // 新しく領域確保
  byte *data = (byte*)new_(this_->data_size + 4);
  // 先頭４バイトにヘッダをいれる
  data[0] = 0xFF;
  data[1] = 0xD9;
  data[2] = 0xFF;
  data[3] = 0xD8;
  // JPEGデータをそのままコピー
  for( i=0 ; i<this_->data_size ; i++ ){
	data[i+4] = this_->raw_data[i];
  }
  // 古い領域を削除
  byte *tmp = this_->raw_data;
  delete_(tmp);
  // ポインタつなぎ変え
  this_->raw_data = data;
  // 長さ更新
  this_->data_size += 4;
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

int main(){
  
  ImageData img;
  if( open_ImageFile(&img, "TestData/img0.jpg") != 0 ){
	printf("error 0\n");
	return 0;
  }
  printf( "size: (%d,%d)\n", img.width, img.height);

  return 0;
}



#endif

