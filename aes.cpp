// created date : 2011/12/13 22:47:44
// last updated : 2012/03/22 11:49:04
// linux: -I/usr/local/ssl/include -L/usr/local/ssl/lib -lcrypto
#include "aes.h"

#include "util.h"
#include <cstdio> // for EOF
#define CONT_NAME "nomlib"

namespace nl{

  /// AES暗号/復号
  static std::stringstream *aes_core(bool encode, std::istream &data, const BYTE key[16], const BYTE iv_org[16]){
	std::stringstream *ret = NULL;
	BYTE iv[16];
	for(int i=0;i<16;i++) iv[i] = iv_org[i];

#if defined(_WIN32)
	const int key_len = 16;
	HCRYPTPROV hProv;
	HCRYPTKEY hKey;
	DWORD	mode = CRYPT_MODE_CBC;
	struct KeyBLOB{
	  BLOBHEADER hdr;
	  DWORD cbKeySize;
	  BYTE rgbKeyData[key_len]; // 16:AES_128のキー長
	}kb;
	kb.hdr.bType = PLAINTEXTKEYBLOB;
	kb.hdr.bVersion = CUR_BLOB_VERSION;
	kb.hdr.reserved = 0;
	kb.hdr.aiKeyAlg = CALG_AES_128;	// AES_128
	kb.cbKeySize = key_len;
	CopyMemory( kb.rgbKeyData, key, key_len );
  
	if(   !CryptAcquireContext(&hProv, CONT_NAME, NULL, PROV_RSA_AES, 0              ))
	  if( !CryptAcquireContext(&hProv, CONT_NAME, NULL, PROV_RSA_AES, CRYPT_NEWKEYSET)) return ERRP("CryptAcquireContext error")	, ret;
	//if( !CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, 0)	) return ERRP("CryptAcquireContext error")	, ret;
	if( !CryptImportKey(hProv,(BYTE*)&kb, sizeof(kb),0,0,&hKey) 					) return ERRP("CryptImportKey error")		, ret;
	if( !CryptSetKeyParam(hKey, KP_IV, iv, 0)										) return ERRP("CryptSetKeyParam error")		, ret;
	if( !CryptSetKeyParam(hKey, KP_MODE, (BYTE*)&mode, 0)							) return ERRP("CryptSetKeyParam error\n")	, ret;
	ret = new std::stringstream();
	// 復号
	{
	  long before = 0, after = 0; // 変換前/変換後のバイト数
	  const unsigned int BUF_LEN = 256;
	  BYTE	pbData[BUF_LEN];
	  DWORD	dwDataLen = BUF_LEN;
	  int tmp;
	
	  while(!data.eof()){
		// nバイト読み出し
		for( dwDataLen = 0; dwDataLen<128 && (tmp = data.get())!=EOF; dwDataLen++ ) pbData[dwDataLen] = tmp;
		//data.read( (char*)pbData, dwDataLen ); // read()はどこまで読めたか分からないため不採用
		if(dwDataLen == 0) break;
		before += (int)dwDataLen;
	  
		(void)data.get(); // 終端ならばeofフラグが立つように get()
		// encode ? Encrypt : Decrypt
		// 3つめの引数 : 最後のデータのときTRUE
		if( encode && !CryptEncrypt(hKey, 0, data.eof(), 0, pbData, &dwDataLen, (DWORD)BUF_LEN) ){ ERRP("CryptEncrypt error"); break; } // 暗号化
		if(!encode && !CryptDecrypt(hKey, 0, data.eof(), 0, pbData, &dwDataLen)){ ERRP("CryptDecrypt error "<<dwDataLen); break; } // 復号化
		data.unget(); // get() した分もどす
	  
		ret->write((const char*)pbData, dwDataLen);
		//for(unsigned int i=0;i<dwDataLen;i++) printf("%c",pbData[i]);
		after += (int)dwDataLen;
	  }
	}
	// 後処理
	CryptDestroyKey( hKey );
	CryptReleaseContext(hProv, 0);
#else
	AES_KEY enc_key;
	BYTE in [AES_BLOCK_SIZE]; // AES_BLOCK_SIZE: 16
	BYTE out[AES_BLOCK_SIZE];
	ret = new std::stringstream();

	if( encode ){
	  // 暗号化
	  AES_set_encrypt_key(key, 128, &enc_key); // 128bit = 16byte
	  int in_len;
	  int c;
	  bool last = false;
	  while(!last){
		for(in_len = 0; in_len < AES_BLOCK_SIZE && (c=data.get()) != EOF; in_len++) in[in_len] = c;
		if(in_len < AES_BLOCK_SIZE){
		  last = true;
		  int padding = AES_BLOCK_SIZE-in_len;
		  //DBGP("padding: " << padding);
		  for(;in_len<AES_BLOCK_SIZE;in_len++) in[in_len] = padding;
		}
		AES_cbc_encrypt(in, out, AES_BLOCK_SIZE, &enc_key, iv, AES_ENCRYPT);
		ret->write((const char*)out, AES_BLOCK_SIZE);
	  }
	}else{
	  // 復号化
	  AES_set_decrypt_key(key, 128, &enc_key); // 128bit = 16byte
	  int c, out_len;
	  bool last = false;
	  while(!last){
		for(int i = 0; i < AES_BLOCK_SIZE && (c=data.get()) != EOF; i++) in[i] = c;
		last = (data.get()==EOF);
		data.unget();
		AES_cbc_encrypt(in, out, AES_BLOCK_SIZE, &enc_key, iv, AES_DECRYPT);
		out_len = last ? (AES_BLOCK_SIZE-out[AES_BLOCK_SIZE-1]) : AES_BLOCK_SIZE; // padding 削除
		ret->write((const char*)out, out_len);
	  }
	  
	}
#endif
	return ret;
  }

  /**
   * AES暗号 128bit, CBC, PKCS5
   * @param data 元データ
   * @param key  パスワード(16bit)
   * @param iv   Initialize Vector(16bit)
   * @return 暗号化したデータ
   */
  std::stringstream *aes_encode(std::istream &data, const BYTE key[16], const BYTE iv[16]){
	return aes_core(true,  data, key, iv );
  }
  /**
   * AES復号 128bit, CBC, PKCS5
   * @param data 暗号化されたデータ
   * @param key  パスワード(16bit)
   * @param iv   Initialize Vector(16bit)
   * @return 復号化したデータ or NULL
   */
  std::stringstream *aes_decode(std::istream &data, const BYTE key[16], const BYTE iv[16]){
	return aes_core(false, data, key, iv );
  }

}; // namespace nl



#ifdef TEST
// test
// $ make TEST=aes
#include <iostream>
#include <fstream>
using namespace std;

int main(){
  unsigned char key[16] = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5};
  unsigned char iv[16]  = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5};

  // 文字列を encode -> decode して出力
  {
	std::stringstream ss;
	ss << "hello, world!!";
	
	std::stringstream *enc = nl::aes_encode( ss , key, iv);
	if(enc == NULL){
	  DBGP("encode error.");
	  return 0;
	}
	std::stringstream *dec = nl::aes_decode(*enc, key, iv);
	
	delete enc;
	
	if(dec == NULL){
	  DBGP("decode error.");
	  return 0;
	}
	DBGP(endl << "'" << dec->str() << "'");
	delete dec;
  }
  
  // ファイルを encode して保存, decode して保存
  {
	int c;
	std::ifstream ifs("TestData/input.txt", std::ios::in | std::ios::binary);
	std::stringstream *enc = nl::aes_encode(ifs, key, iv);
	if(enc == NULL){
	  DBGP("encode error.");
	  return 0;
	}
	DBGP("encode TestData/input.txt");
	
	std::ofstream ofs1("TestData/input.enc", std::ios::out | std::ios::binary);
	std::stringstream tmp;
	while( (c=enc->get())!=EOF){ ofs1.put(c); tmp.put(c); }
	DBGP("save to TestData/input.enc");
	delete enc;

	std::stringstream *dec = nl::aes_decode(tmp, key, iv);
	if(dec == NULL){
	  DBGP("decode error.");
	  return 0;
	}
	std::ofstream ofs2("TestData/input.dec", std::ios::out | std::ios::binary);
	while( (c=dec->get())!=EOF) ofs2.put(c);
	DBGP("save to TestData/input.dec");
	delete dec;
  }
  
  return 0;
}
#endif
