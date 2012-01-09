// -*- mode:c++ -*-
#ifndef NL_VARIABLE_H
#define NL_VARIABLE_H
// created date : 2011/12/18 22:43:33
// last updated : 2012/01/10 02:25:16
// 動的型 dynamic type

#include <string>
#include <tr1/memory>

namespace nl{
  
  class AbsFunction;  // 抽象クラス: 関数
  class AbsNameTable; // 抽象クラス: 名前表
  class Variable;     // 動的型 変数。 AbsFunction, AbsNameTable へのポインタを持つ

  typedef std::tr1::shared_ptr<Variable> Variable_Ptr;

  // 抽象クラス: 関数を表す
  class AbsFunction{
  public:
	typedef std::tr1::shared_ptr<AbsFunction> Ptr;
	static const Ptr NullPtr;

	virtual ~AbsFunction(){}
	virtual const std::string &name() const = 0;
  };

  // 抽象クラス: 名前表インタフェース
  class AbsNameTable{
  public:
	typedef std::tr1::shared_ptr<AbsNameTable> Ptr;
	static const Ptr NullPtr;

	AbsNameTable();
	virtual ~AbsNameTable();
	virtual const std::string &name() const = 0; // 名前表の名前(識別子)
	
	bool add(const std::string &name, Variable *var){ return add(name, Variable_Ptr(var)); }
	virtual bool add(const std::string &name, Variable_Ptr var) = 0;
	virtual Variable_Ptr find(const std::string &name) = 0;
	virtual Variable_Ptr find(unsigned int idx) = 0;
	Variable_Ptr find(const Variable &var);

	virtual void dump() = 0;

	static Ptr push(Ptr parent, Ptr child);
	static Ptr pop (Ptr nt);
	
  public:
	// 親へのポインタ
	Ptr parent_;
  };


  // 動的型 変数
  class Variable{
  public:
	typedef std::tr1::shared_ptr<Variable> Ptr;
	static const Ptr NullPtr;
	// 変数の型
	//  型が違った場合などに どうするか(エラーにするか?警告か?) は処理系依存。
	//  オーバーロードを利用して 型推論する。
	typedef enum{
	  TypeMissMatch = -1, // fitType() の戻り値用 
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
	Variable();
	// #undef な Variable を生成
	static Variable undef(){ return Variable(); }
	// true/false
	static Variable True() { return Variable("true"); }
	static Variable False(){ return undef(); }

	explicit Variable(int val);
	explicit Variable(const std::string &val);
	explicit Variable(Variable::Ptr     p);
	explicit Variable(AbsFunction::Ptr  p);
	explicit Variable(AbsNameTable::Ptr p);
	Variable(Type type, const std::string &val); // val を指定したTypeに変換して初期化
	virtual ~Variable();
	
	Variable(const Variable &obj);
	Variable &operator=(const Variable &obj);

	/// assign
	Variable &assign_undef(); // undef を代入
	Variable &assign(int val);
	Variable &assign(const std::string &val);
	Variable &assign(AbsNameTable::Ptr p);
	Variable &assign(const Variable &obj);
	Variable &assign(Type type, const std::string &val); // val を指定したTypeに変換して代入

	// accessor
	void type(Type t) { type_ = t; }
	Type type() const{ return type_; }
	int asInt() const;
	std::string asStr() const;
	bool asBool() const;
	Variable::Ptr     ptrV()  const{ return (type_==Pointer)?ptr_v :Variable::NullPtr; };
	AbsFunction::Ptr  ptrF()  const{ return (type_==Pointer)?ptr_f :AbsFunction::NullPtr; };
	AbsNameTable::Ptr ptrNT() const{ return (type_==Pointer)?ptr_nt:AbsNameTable::NullPtr; };

	bool isFunction()  const{ return ptrF();  }
	bool isNameTable() const{ return ptrNT(); }
	
	// for debug
	void dump() const;
	std::string dump_str() const;
	
	/// 演算
	Variable  operator+ (const Variable& o){ return oper("+",o);        };
	Variable& operator+=(const Variable& o){ return asgn_oper("+=", o); }
	// except を true にすると、エラー時に throw 0; する。
	Variable &asgn_oper(const std::string &op, const Variable &o, bool except = false); // 代入系
	Variable oper(const std::string &op, const Variable &o, bool except = false) const; // 非代入
	

  private:	/// member
	Type type_;
	
	int val_int;
	std::string val_str;
	Variable::Ptr     ptr_v;
	AbsFunction::Ptr  ptr_f; // 関数へのポインタ
	AbsNameTable::Ptr ptr_nt; // 名前表へのポインタ
	
	// flag
	bool constant; // 定数?
  };
  
}; // namespace nl

#endif
