#ifndef _NOMLIB_IMAGE_READER_H_
#define _NOMLIB_IMAGE_READER_H_
// created date : 2012/01/26 15:09:13
// last updated : 2012/01/26 15:10:05
// 以前作ってたやつを持ってきた

#ifdef __cplusplus
namespace nl{
#endif


// 画像ファイルを読み込む
typedef unsigned char byte;

enum ImageType{ Type_NONE, Type_JPEG, Type_PNG, Type_GIF };
typedef struct ImageData_{
  byte *raw_data;  // ファイルの生データ
  unsigned int data_size;   // ファイルサイズ
  
  enum ImageType type;	// 画像ファイルのフォーマット
  int width; // 画像の横幅、縦幅
  int height;
}ImageData;

// 画像ファイルを読み込み、
// type,width,heightを計算
// @param file_name 読み込む画像ファイル名
// @return 正常時 0 , エラー時 -1
int open_ImageFile(ImageData *this_,const char *file_name);

// JPEGの 'erroneous header' (0xFF 0xD9 0xFF 0xD8) を先頭に追加
// 参考: SWF V10 仕様書 P.148
void addErroneousHeader_ImageFile(ImageData *this_);

//void readPNG(ImageData *data);

#ifdef __cplusplus
}; // namespace nl
#endif

#endif
