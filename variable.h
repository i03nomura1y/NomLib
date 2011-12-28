// -*- mode:c++ -*-
#ifndef NL_VARIABLE_H
#define NL_VARIABLE_H
// created date : 2011/12/18 22:43:33
// last updated : 2011/12/29 03:17:09
// 動的型 dynamic type

#include <string>

namespace nl{
  
  class AbsFunction;  // 抽象クラス: 関数
  class AbsNameTable; // 抽象クラス: 名前表
  class Variable;     // 動的型 変数。 AbsFunction, AbsNameTable へのポインタを持つ


  // 抽象クラス: 関数を表す
  class AbsFunction{
  public:
	virtual ~AbsFunction(){}
	virtual const std::string &name() const = 0;
  };

  // 抽象クラス: 名前表インタフェース
  class AbsNameTable{
  public:
	virtual ~AbsNameTable(){}
	virtual const std::string &name() const = 0; // 名前表の名前(識別子)
	//virtual bool add(const std::string &name, nl::Variable &var) = 0;	// 追加
	//virtual Variable *find(const std::string &name) = 0;	// 取得
  };


  // 動的型 変数
  class Variable{
  public:
	// 変数の型
	//  型が違った場合などに どうするか(エラーにするか?警告か?) は処理系依存。
	//  オーバーロードを利用して 型推論する。
	typedef enum{
	  Undef = 0, // 未定義
	  Integer,
	  String,
	  Pointer, // void* しか考えない。関数, 名前表 の場合もこの値になる
	}Type;
	
  private:
	// undef のときの値
	static const int undef_int;
	static const std::string undef_str;
	
  public:
	// undef で初期化。vector<> のために public にする
	Variable() : type_(Undef), val_int(undef_int), val_str(undef_str), ptr_v(NULL), ptr_f(NULL), ptr_nt(NULL), constant(false){}
	// #undef な Variable を生成
	static Variable undef(){ return Variable(); }

	explicit Variable(int val) : type_(Integer), val_int(val), val_str(undef_str), ptr_v(NULL), ptr_f(NULL), ptr_nt(NULL), constant(false){}
	explicit Variable(const std::string &val) : type_(String), val_int(undef_int), val_str(val), ptr_v(NULL), ptr_f(NULL), ptr_nt(NULL), constant(false){}
	explicit Variable(Variable *var) : type_(Pointer), val_int(undef_int), val_str(undef_str), ptr_v(var), ptr_f(NULL), ptr_nt(NULL), constant(false){}
	explicit Variable(AbsFunction *func) : type_(Pointer), val_int(undef_int), val_str(undef_str), ptr_v(NULL), ptr_f(func), ptr_nt(NULL), constant(false){}
	Variable(Type type, const std::string &val); // val を指定したTypeに変換して初期化
	~Variable(){}
	
	Variable(const Variable &obj)
	  : type_(obj.type_), val_int(obj.val_int), val_str(obj.val_str),
		ptr_v(obj.ptr_v), ptr_f(obj.ptr_f), ptr_nt(obj.ptr_nt), 
		constant(obj.constant){}
	Variable &operator=(const Variable &obj){
	  type_=obj.type_;
	  val_int = obj.val_int; val_str = obj.val_str;
	  ptr_v = obj.ptr_v; ptr_f = obj.ptr_f; ptr_nt = obj.ptr_nt; 
	  constant = obj.constant;
	  return *this;
	}

	void assign_undef(){                 type_=Undef;   val_int = undef_int; val_str = undef_str; ptr_v = NULL; ptr_f = NULL; ptr_nt = NULL; }
	void assign(int val){                type_=Integer; val_int = val;       val_str = undef_str; ptr_v = NULL; ptr_f = NULL; ptr_nt = NULL;  }
	void assign(const std::string &val){ type_=String;  val_int = undef_int; val_str = val;       ptr_v = NULL; ptr_f = NULL; ptr_nt = NULL;  }
	void assign(Type type, const std::string &val); // val を指定したTypeに変換して代入
	
	// accessor
	void type(Type t) { type_ = t; }
	Type type() const{ return type_; }
	int asInt() const;
	std::string asStr() const;
	Variable     *ptrV()  const{ return (type_==Pointer)?ptr_v :NULL; };
	AbsFunction  *ptrF()  const{ return (type_==Pointer)?ptr_f :NULL; };
	AbsNameTable *ptrNT() const{ return (type_==Pointer)?ptr_nt:NULL; };

	bool isFunction() const{ return (type_==Pointer && ptr_f!=NULL); }
	
	// for debug
	void dump() const;
	std::string dump_str() const;


  private:
	Type type_;
	
	int val_int;
	std::string val_str;
	Variable     *ptr_v;
	AbsFunction  *ptr_f; // 関数へのポインタ
	AbsNameTable *ptr_nt; // 名前表へのポインタ
	
	// flag
	bool constant; // 定数?
  };
  
}; // namespace nl

#endif
