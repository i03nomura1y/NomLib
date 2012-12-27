// -*- mode: cpp -*-
// 参考 http://blog.majide.com/2009/03/c-regex-sample-code/
// コンパイルオプション: -lregex
#include "regex.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include "util.h"

namespace nl{
    RegEx::RegEx() : valid(false), str(),idx_list(){}
    
    RegEx::RegEx(const std::string &rxstr) : valid(false), str(),idx_list(){
        compile(rxstr);
    }

    // コピーコンストラクタ
    RegEx::RegEx(const RegEx &obj) : valid(false), str(obj.str), idx_list(obj.idx_list) {
        compile(obj.m_rx_str);
    }
    // 代入演算子
    RegEx &RegEx::operator=(const RegEx &obj){
        compile(obj.m_rx_str);
        str = obj.str;
        idx_list = obj.idx_list;
        return *this;
    }

    RegEx::~RegEx(){
        idx_list.clear();
        if(valid) regfree(&m_reg);
    }
  
    void RegEx::swap(RegEx &obj){
        using std::swap;
        swap(valid   , obj.valid   );
        swap(m_reg   , obj.m_reg   );
        swap(m_rx_str, obj.m_rx_str);
        swap(str     , obj.str     );
        swap(idx_list, obj.idx_list);
    }

    // 正規表現文字列をコンパイルする
    // @param rxstr 正規表現 文字列
    bool RegEx::compile(const std::string &rxstr){
        if(valid) regfree(&m_reg);
        // regcomp の最後の引数
        //   REG_EXTENDED : POSIX拡張正規表現を使用。指定なしなら POSIX 標準正規表現が使われる
        //   REG_ICASE    : 大文字小文字の違いを無視
        //   REG_NOSUB    : regexec で パラメータ nmatch, pmatch が無視される
        //   REG_NEWLINE  : 全ての文字にマッチするオペレータに改行をマッチさせない
        int ret = regcomp(&m_reg, rxstr.c_str(), REG_EXTENDED | REG_NEWLINE );
        valid = (ret == 0); // コンパイルに成功したら true
        m_rx_str = rxstr;
        if( !valid ){
            char errbuf[100];
            regerror(ret, &m_reg, errbuf, sizeof errbuf);
            ERRP("regcomp error:" << errbuf);
            ERRP("  rx str: '" << rxstr << "'");
        }
        return valid;
    }
        
    bool RegEx::match(const std::string &str_){
        if( !valid ){ ERRP("invalid regex"); return false; }
	
        size_t nmatch = 100;
        regmatch_t pmatch[nmatch];
        idx_list.clear();
        str = str_;
        // regexec の最後の引数
        //   REG_NOTBOL : 行頭にマッチするオペレータは必ずマッチに失敗する
        //                (複数行文字列の先頭を行頭として解釈させない場合に用いる)
        //   REG_NOTEOL : 行末にマッチするオペレータは必ずマッチに失敗する
        if(valid && (regexec(&m_reg, str_.c_str() , nmatch, pmatch, 0) == REG_NOERROR) ){
            for (unsigned int i = 0; i < nmatch && pmatch[i].rm_so>=0; i++) {
                int so = (int)pmatch[i].rm_so;
                int eo = (int)pmatch[i].rm_eo;
                //std::cout << i << " " << so << " " << eo << " " << (eo-so) << std::endl;
                idx_list.push_back( std::pair<int,int>( so, eo-so ) );
            }
            //std::cout << "size " << idx_list.size() << std::endl;
            return true;
        }
        return false;
    }
    /// 前回の実行によりマッチした部分文字列の idx 番目のものを返す
    std::string RegEx::get(unsigned int idx) const{
        if( idx>=idx_list.size() ){
            ERRP("size "<< idx_list.size() << "  arg " << idx);
            throw std::out_of_range("in RegEx::get(int)");
        }
        //std::cout << idx_list[idx].first << "," << idx_list[idx].second << std::endl;
        //std::cout << str << "," << (str.substr(0,1)) << std::endl;
        return str.substr( idx_list[idx].first, idx_list[idx].second );
    }
    int RegEx::spos(unsigned int idx) const{
        if( idx>=idx_list.size() ){
            ERRP("size "<< idx_list.size() << "  arg " << idx);
            throw std::out_of_range("in RegEx::spos(int)");
        }
        return idx_list[idx].first;
    }
    int RegEx::epos(unsigned int idx) const{
        if( idx>=idx_list.size() ){
            ERRP("size "<< idx_list.size() << "  arg " << idx);
            throw std::out_of_range("in RegEx::epos(int)");
        }
        return ( idx_list[idx].first + idx_list[idx].second );
    }

    /// 置換
    // src 内のマッチした部分文字列を repl で置き換える
    std::string RegEx::replace(const std::string &src, const std::string &repl, int idx){
        std::stringstream ss;
        unsigned int i;
        for(i = 0; i<src.length() && match(&src[i]); i += epos(idx)){
            //DBGP("'" << ss.str() << "' <- '" << &src[i] << "'");
            //DBGP("match  " << (re.spos(idx)) << " , " << (re.epos(idx)));
            //DBGP("'" << src.substr(i, re.spos(idx)) << "'");
            ss << src.substr(i, spos(idx)) << repl;
        }
        if(i<src.length()) ss << src.substr(i);
        //DBGP("'" << ss.str() << "' <- '" << &str[i] << "'");
        return ss.str();
    }
  
    /*
    // sample: \$ -> $
    std::string RegEx::replace_dollar( const std::string &str ){
	RegEx re("\\\\.");
	std::stringstream ss;
	unsigned int i;
	for(i = 0; i<str.length() && re.match(&str[i]); i += re.epos(0)){
    if(re.get(0)[1] == '$') ss << str.substr(i, re.spos(0)) << "$";
    else                    ss << str.substr(i, re.epos(0));
	}
	if(i<str.length()) ss << str.substr(i);
	return ss.str();
	}//*/

  
};

#ifdef TEST
// $ make TEST=regex
#include <iostream>
using namespace std;
using nl::RegEx;

void test(const string &rx_str, const string &str1){
    RegEx re(rx_str);

    if( !re.isValid() ){ cerr << "error: invalid RegEx" << endl; return; }
  
    cout << " ================================================" << endl;
    cout << "rx  : " << re.rx_str() << endl;
    cout << "str : '" << str1 << "'" << endl;
    if( re.match(str1) ){
        for(unsigned int i=0; i<re.length(); i++){
            cout << " [" << i << "] " << re.get(i) << " (" << re.spos(i) << "-" << re.epos(i) << ")" << endl;
        }
    }else{
        cout << " no match" << endl;
    }
}

int main(){
    // match のテスト
    //test("hello",          "hello, world!!");
    //test("(\\w*), (\\w*)", "hello, world!!");
    //test("<.*>", "<aaaa><bbbb>");
    test("<[^>]*>", "<aaaa><bbbb>");

    // replace のテスト
    cout << " ================================================" << endl;
    cout << "replace_test:" << endl;
    std::string in = "Good morning! morning!";
    RegEx re("morning");
    cout << in << endl;
    // re にマッチした部分を置き換える。
    cout << " ->" << re.replace(in, "evening") << endl;
    
    return 0;
}
#endif
