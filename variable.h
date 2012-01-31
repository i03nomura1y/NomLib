// -*- mode:c++ -*-
#ifndef NL_VARIABLE_H
#define NL_VARIABLE_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/02/01 07:26:41
// 動的型 dynamic type

#include <string>
#include <vector>
#include "ptr_base.h"

namespace nl{
  class AbsFunction;  // 抽象クラス: 関数
  class AbsNameTable; // 抽象クラス: 名前表
  class Variable;     // 動的型 変数。 AbsFunction, AbsNameTable へのポインタを持つ

  typedef typename hasPtr<Variable>::Ptr     PtrV;
  typedef typename hasPtr<AbsFunction>::Ptr  PtrF;
  typedef typename hasPtr<AbsNameTable>::Ptr PtrNT;
  typedef std::vector<Variable> Arguments; // 実引数リスト(Function用)
  typedef std::pair<std::string, PtrV> NTEntry; // 名前と値のペア(NameTable用)

  // 抽象クラス: 関数を表す
  class AbsFunction : public hasPtr<AbsFunction>{
  public: static const Ptr NullPtr;
  public:
	virtual ~AbsFunction(){}
	virtual const std::string &name() const = 0;
	// 渡された実引数リストを実行できるか ( = 引数の個数・型が正しいか)
	virtual bool acceptable(const Arguments &args) const = 0;
  };

  // 抽象クラス: 名前表インタフェース
  class AbsNameTable : public hasPtr<AbsNameTable>{
  public:
	static const Ptr NullPtr;

  public:
	AbsNameTable();
	virtual ~AbsNameTable();
	virtual PtrNT clone() const = 0;	// clone (Deep Copy)
	virtual const std::string &name() const = 0; // 名前表の名前(識別子)
	virtual int size() const = 0; // 登録されている変数の数
	
	template <class T>
	PtrV add(const T &name, Variable *var){ return add(name, PtrV(var)); }
	virtual PtrV add (const Variable    &name, PtrV var);
	virtual PtrV add (const std::string &, PtrV ) = 0;
	virtual PtrV add (const int, PtrV) = 0;
	virtual PtrV find(const Variable    &name);
	virtual PtrV find(const std::string &) = 0;
	virtual PtrV find(const int) = 0;
	
	virtual void dump() = 0;
	void dump_stack();
	
	static Ptr push(Ptr parent, Ptr child);
	static Ptr pop (Ptr nt);
	
  public:
	// 親へのポインタ
	Ptr parent_;
  };


  // 動的型 変数
  class Variable : public hasPtr<Variable>{
  public: // alias, const
	static const Ptr NullPtr;
	// alias
	typedef unsigned char uchar;
	typedef double dbl;
	// 変数の型
	//  型が違った場合などに どうするか(エラーにするか?警告か?) は処理系依存。
	//  オーバーロードを利用して 型推論する。
	typedef enum{
	  TypeMissMatch = -1, // fitType() の戻り値用 
	  Undef = 0, // 未定義
	  Auto,    // 代入されたときに決定。挙動はUndefと同じ
	  Integer,
	  Float,   // 小数
	  String,
	  Boolean, // 内部の値は int。
	  VoidPtr, // Variable へのポインタ。void* しか考えない
	  FuncPtr, // 関数
	  Array  , // 名前表
	}Type;
	
  private: // const
	// undef のときの値
	static const long  undef_int;
	static const std::string undef_str;
	
  public: // method
	// undef で初期化。vector<> のために public にする
	Variable();
	// #undef な Variable を生成
	static Variable undef(){ return Variable(); }
	
	explicit Variable(const int   &v);
	explicit Variable(const long  &v);
	explicit Variable(const dbl   &v);
	explicit Variable(const bool  &v);
	explicit Variable(const char  *v);
	explicit Variable(const uchar *v);
	explicit Variable(const std::string &v);
	explicit Variable(const PtrV  &p);
	explicit Variable(const PtrF  &p);
	explicit Variable(const PtrNT &p);
	Variable(const Variable &obj);
	Variable(Type type, const std::string &val); // val を指定したTypeに変換して初期化
	Variable(Type type, const Variable    &val); // val を指定したTypeに変換して代入
	virtual ~Variable();
	
	Variable &operator=(const Variable &obj);

	/// assign
	Variable &assign_undef(); // undef を代入
	Variable &assign(const int  &v);
	Variable &assign(const long &v);
	Variable &assign(const dbl  &v);
	Variable &assign(const bool &v);
	Variable &assign(const char *v);
	Variable &assign(const unsigned char *v);
	Variable &assign(const std::string &v);
	Variable &assign(const PtrNT &p);
	Variable &assign(const Variable &obj);
	Variable &assign(Type type, const std::string &val); // val を指定したTypeに変換して代入
	Variable &assign(Type type, const Variable    &val); // val を指定したTypeに変換して代入
	Variable &assign_auto(const std::string &val); // val の型を自動判別して代入
	
	/// clone (Deep Copy)
	PtrV clone() const;
	
	// accessor
	void type(Type t) { type_ = t; }
	Type type() const{ return type_; }
	int asInt() const;
	std::string asStr() const;
	const std::string &refOf_val_str() const{ return val_str; } // 
	bool asBool() const;
	PtrV  ptrV()  const{ return (type_==VoidPtr)?ptr_v :Variable::NullPtr; };
	PtrF  ptrF()  const{ return (type_==FuncPtr)?ptr_f :AbsFunction::NullPtr; };
	PtrNT ptrNT() const{ return (type_==Array  )?ptr_nt:AbsNameTable::NullPtr; };

	bool isFunction()  const{ return ptrF();  }
	bool isNameTable() const{ return ptrNT(); }
	
	// for debug
	void dump() const;
	std::string dump_str() const;
	
	/// 演算
	//Variable  operator+ (const Variable& o){ return oper("+",o);        };
	//Variable& operator+=(const Variable& o){ return asgn_oper("+=", o); }
	// except を true にすると、エラー時に throw 0; する。
	Variable &asgn_oper(const std::string &op, const Variable &o, bool except = false); // 代入系
	Variable  asgn_oper(const std::string &op, bool except = false); // 前置 単項演算子
	Variable oper(const std::string &op, const Variable &o, bool except = false) const; // 非代入
	Variable oper(const std::string &op, bool except = false) const; // 前置 単項演算子
	

  private:	/// member
	Type type_;
	
	long val_int;
	double val_dbl;
	std::string val_str;
	PtrV  ptr_v;
	PtrF  ptr_f; // 関数へのポインタ
	PtrNT ptr_nt; // 名前表へのポインタ
	
	// flag
	bool constant; // 定数?
  };
  
}; // namespace nl

#endif
