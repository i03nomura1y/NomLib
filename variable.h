// -*- mode:c++ -*-
#ifndef NL_VARIABLE_H
#define NL_VARIABLE_H
// created date : 2011/12/18 22:43:33
// last updated : 2011/12/25 20:44:26
// 動的型 dynamic type

#include <string>

namespace nl{
  
  class Variable{
  public:
	// 変数の型
	//  型が違った場合などに どうするか(エラーにするか?警告か?) は処理系依存。
	//  オーバーロードを利用して 型推論する。
	typedef enum{
	  Undef = 0, // 未定義
	  Integer,
	  String,
	  Pointer, // void* しか考えない
	}Type;
	
  private:
	// undef のときの値
	static const int undef_int;
	static const std::string undef_str;
	
  private:
	// undef で初期化
	Variable() : type_(Undef), val_int(undef_int), val_str(undef_str), constant(false), function(false){}
  public:
	explicit Variable(int val) : type_(Integer), val_int(val), val_str(undef_str), constant(false), function(false){}
	explicit Variable(const std::string &val) : type_(String), val_int(undef_int), val_str(val), constant(false), function(false){}
	Variable(Type type, const std::string &val); // val を指定したTypeに変換して初期化
	~Variable(){}
	
	Variable(const Variable &obj)
	  : type_(obj.type_), val_int(obj.val_int), val_str(obj.val_str),
		constant(obj.constant), function(obj.function){}
	Variable &operator=(const Variable &obj){
	  type_=obj.type_; val_int = obj.val_int; val_str = obj.val_str;
	  constant = obj.constant;
	  function = obj.function;
	  return *this;
	}

	void assign(int val){ type_=Integer; val_int = val; function = false; }
	void assign(const std::string &val){ type_ = String; val_str = val; function = false; }
	void assign(Type type, const std::string &val); // val を指定したTypeに変換して代入
	
	// getter
	Type type() const{ return type_; }
	int asInt() const;
	std::string asStr() const;

	void dump();

	// #undef な Variable を生成
	static Variable undef(){ return Variable(); }

  private:
	Type type_;
	int val_int;
	std::string val_str;
	
	// flag
	bool constant; // 定数?
	bool function; // 関数を表す?
  };
  
}; // namespace nl

#endif
