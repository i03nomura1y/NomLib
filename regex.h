// -*- mode: cpp -*-
// regex.h の c++ 版
// -lregex
#ifndef __NOMLIB_REGEX_H__
#define __NOMLIB_REGEX_H__

#include <regex.h>
#include <string>
#include <vector>

namespace nl{
    class RegEx{
    public:
        // デフォルトコンストラクタ
        // compile() を明示的に呼びだすまで invalid 状態
        RegEx();
        // 正規表現文字列をコンパイルする
        // @param rxstr 正規表現 文字列
        // [\x20-\x7F]  0x20 - 0x7F の一文字にマッチ。 '\' はひとつだけでOK
        explicit RegEx(const std::string &rxstr);
        virtual ~RegEx();
        RegEx(const RegEx &obj); // コピーコンストラクタ
        RegEx &operator=(const RegEx &obj); // 代入演算子
        void swap(RegEx &obj);
    public:
        // 正規表現文字列をコンパイルする
        // @param rxstr 正規表現 文字列
        bool compile(const std::string &rxstr);
        // コンパイル時に指定された正規表現文字列を返す
        std::string rx_str() const{ return m_rx_str; };
        // 正規表現を正常にコンパイルできているか？
        bool isValid() const { return valid; }
	
        /// 正規表現にマッチする?
        // @return  マッチしたら true
        bool match(const std::string &str_);
        
        // マッチした部分文字列の個数
        unsigned int length() const{ return idx_list.size(); }
	
        /// 前回の実行によりマッチした部分文字列の idx 番目のものを返す
        std::string get(unsigned int idx) const; // 部分文字列
        int spos(unsigned int idx) const; // 開始インデクス
        int epos(unsigned int idx) const;   // 終了インデクス

        /// 置換
        // src 内のマッチした部分文字列を repl で置き換える
        // @param src  元の文字列
        // @param repl 置き換え後の部分文字列
        // @param idx  何番目の match を置き換えるか
        // @return 置き換え後の文字列
        std::string replace(const std::string &src, const std::string &repl, int idx = 0);

    private:
        bool valid;
        regex_t m_reg;
        std::string m_rx_str; // コンストラクタに渡された文字列
        // 前回 match()実行したときの文字列
        std::string str;
        // 前回の match()実行によりマッチした部分文字列の(インデクス,長さ) ただし最大100個
        std::vector<std::pair<int,int> > idx_list;
	
    };
}; // namespace nl

#endif

