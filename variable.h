// -*- mode:c++ -*-
#ifndef NL_VARIABLE_H
#define NL_VARIABLE_H
// created date : 2011/12/18 22:43:33
// last updated : 2011/12/19 03:01:25
// 動的型 dynamic type

#include <string>

namespace nl{
  
  class Variable{
  public:
	// 変数の型
	//  型が違った場合などに どうするか(エラーにするか?警告か?) は処理系依存。
	//  オーバーロードを利用して 型推論する。
	typedef enum{
	  undef = 0, // 未定義
	  Integer,
	  String,
	}Type;
	
  private:
	// undef のときの値
	static int undef_int;
	static std::string undef_str;
	
  public:
	Variable() : type_(undef), val_int(undef_int), val_str(undef_str){}
	explicit Variable(int val) : type_(Integer), val_int(val), val_str(undef_str){}
	explicit Variable(const std::string &val) : type_(String), val_int(undef_int), val_str(val){}
	Variable(Type type, const std::string &val); // val を指定したTypeに変換して初期化
	~Variable(){}
	
	Variable(const Variable &obj) : type_(obj.type_), val_int(obj.val_int), val_str(obj.val_str){}
	Variable &operator=(const Variable &obj){ type_=obj.type_; val_int = obj.val_int; val_str = obj.val_str; return *this;}

	void assign(int val){ type_=Integer; val_int = val; }
	void assign(const std::string &val){ type_ = String; val_str = val; }
	void assign(Type type, const std::string &val); // val を指定したTypeに変換して代入
	
	// getter
	Type type() const{ return type_; }
	int asInt() const;
	std::string asStr() const;

	void dump();

  private:
	Type type_;
	int val_int;
	std::string val_str;
  };
  
}; // namespace nl

#endif
