// -*- mode:cpp -*-
// created date : 2011/12/13 22:47:44
// last updated : 2012/10/18 18:32:14
// linux: -I/usr/local/ssl/include -L/usr/local/ssl/lib -lcrypto
#ifndef __NOMLIB_AES_H__
#define __NOMLIB_AES_H__


#if defined(_WIN32)
//#  ifndef WINVER
//#    define WINVER 0x0601
//#    define _WIN32_WINNT 0x0601
//#  endif
#  include <Windows.h>
#  include <WinCrypt.h>
#else
#  include <openssl/aes.h>
//  -I/usr/local/ssl/include -L/usr/local/ssl/lib -lcrypto
typedef unsigned char BYTE;
#endif


#include <sstream>

namespace nl{

  enum CryptMode{ CryptMode_CBC, CryptMode_ECB };

  /**
   * AES暗号 128bit, CBC or ECB, PKCS5
   * 戻り値は delete する必要がある
   * @param mode 暗号方式
   * @param data 元データ
   * @param key  パスワード(16bit)
   * @param iv   Initialize Vector(16bit)
   * @return 暗号化したデータ or NULL
   */
  std::stringstream *aes_encode(CryptMode mode,
                                std::istream &data,
                                const BYTE key[16],
                                const BYTE iv[16]);
  /**
   * AES復号 128bit, CBC or ECB, PKCS5
   * 戻り値は delete する必要がある
   * @param mode 暗号方式
   * @param data 暗号化されたデータ
   * @param key  パスワード(16bit)
   * @param iv   Initialize Vector(16bit)
   * @return 復号化したデータ or NULL
   */
  std::stringstream *aes_decode(CryptMode mode,
                                std::istream &data,
                                const BYTE key[16],
                                const BYTE iv[16]);

}; // namespace nl

#endif
