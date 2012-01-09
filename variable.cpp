// created date : 2011/12/18 22:43:33
// last updated : 2012/01/09 22:20:30
// 動的型 dynamic type

#include "variable.h"

#include "util.h"

#include <cstdio> // for sprintf()
#include <cstdlib> // for atoi()
#include <iostream>
using namespace std;

namespace nl{
  static char buf_variable[1024];

  const int    Variable::undef_int = 0;
  const string Variable::undef_str = "#undef";

  const AbsFunction::Ptr  AbsFunction::NullPtr  = AbsFunction::Ptr();
  const AbsNameTable::Ptr AbsNameTable::NullPtr = AbsNameTable::Ptr();
  const Variable::Ptr     Variable::NullPtr     = Variable::Ptr();

  AbsNameTable::AbsNameTable(){ nl_INC(); }
  AbsNameTable::~AbsNameTable(){ nl_DEC(); }


  Variable::~Variable(){ nl_DEC(); }

  // undef で初期化。vector<> のために public にする
  Variable::Variable()                          : type_(Undef  ), val_int(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(int val)                   : type_(Integer), val_int(val      ), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const string &val)         : type_(String ), val_int(undef_int), val_str(val      ), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(Variable     *p)           : type_(Pointer), val_int(undef_int), val_str(undef_str), ptr_v(p), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(AbsFunction  *p)           : type_(Pointer), val_int(undef_int), val_str(undef_str), ptr_v( ), ptr_f(p), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(AbsNameTable *p)           : type_(Pointer), val_int(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt(p), constant(false){ nl_INC(); }
  Variable::Variable(Type type, const string &val) : type_(type), val_int(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC();  assign(type, val); }
  Variable::Variable(const Variable &o)         : type_(o.type_), val_int(o.val_int), val_str(o.val_str), ptr_v(o.ptr_v), ptr_f(o.ptr_f), ptr_nt(o.ptr_nt), constant(o.constant){ nl_INC(); }
  Variable &Variable::operator=(const Variable &obj){ return assign(obj); }

  /// assign()
  Variable &Variable::assign_undef()           { type_=Undef;   val_int = undef_int; val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(int val)          { type_=Integer; val_int = val;       val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const string &val){ type_=String;  val_int = undef_int; val_str = val;       ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(AbsNameTable::Ptr p){ type_=Pointer; val_int = undef_int; val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = p;     return *this; }
  Variable &Variable::assign(const Variable &o){ type_=o.type_; val_int = o.val_int; val_str = o.val_str; ptr_v = o.ptr_v; ptr_f = o.ptr_f; ptr_nt = o.ptr_nt; constant = o.constant; return *this; }
  Variable &Variable::assign(Type type, const string &val){  // val を指定したTypeに変換して代入
	type_ = type;
	switch(type_){
	case Undef:   assign_undef(); break;
	case Integer: assign(atoi(val.c_str())); break;
	case String:  assign(val); break;
	default:
	  ERRP("unimplemented.");
	}
	return *this;
  }

  int Variable::asInt() const{
	switch(type_){
	case Undef:   return undef_int;
	case Integer: return val_int;
	default:
	  ERRP("unimplemented.");
	}
	return undef_int;
  }
  std::string Variable::asStr() const{
	switch(type_){
	case Undef: return undef_str;
	case Integer: snprintf(buf_variable, 1023, "%d", val_int); return buf_variable;
	case String: return val_str;
	case Pointer: if(isFunction()) return ptr_f->name();
	default:
	  ERRP("unimplemented.");
	}
	return undef_str;
  }
  
  // 型を合わせる。合わなければ TypeMissMatch
  static Variable::Type fitType(const Variable::Type &lt, const Variable::Type &rt){
	// Undef -> 他方に合わせる
	if( lt == Variable::Undef ) return rt;
	if( rt == Variable::Undef ) return lt;
	// 型が違う
	if( lt != rt ) return Variable::TypeMissMatch;
	return lt;
  }

  // 代入系の演算
  Variable &Variable::asgn_oper(const std::string &op, const Variable &o, bool except){
	// 代入だけ先に処理
	if( op == "=" ){ return assign(o); }
	switch( fitType(type_, o.type_) ){
	case Undef:   return *this;
	case Integer:
	  if( op == "+="){ return assign(val_int + o.val_int); }
	  break;
	case String:
	  if( op == "+="){ return assign(val_str + o.val_str); }
	  break;
	default:
	  break;
	}
	if(except) throw 0;
	ERRP("unimplemented operation '"<< op << "' (" << dump_str() << ", " << o.dump_str() << ")");
	return *this;
  }

  // 非代入系の演算
  Variable Variable::oper(const std::string &op, const Variable &o, bool except) const{
	switch( fitType(type_, o.type_) ){
	case Undef:   return undef();
	case Integer:
	  if( op == "+" ){ return Variable( val_int + o.val_int );  }
	  break;
	case String:
	  if( op == "+" ){ return Variable( val_str + o.val_str );  }
	  break;
	default:
	  break;
	}
	if(except) throw 0;
	ERRP("unimplemented operation '"<< op << "' (" << dump_str() << ", " << o.dump_str() << ")");
	return undef();
  }

  void Variable::dump() const{
	cout << dump_str() << endl;
  }
  std::string Variable::dump_str() const{
	switch(type_){
	case Undef: return "#undef";
	case Integer: snprintf(buf_variable, 1023, "%d", val_int); return "int|"+string(buf_variable);
	case String: return "string|"+val_str;
	case Pointer:
	  return
		(ptr_v !=NULL)?"(pointer: "+ptr_v->dump_str()+")":
		(ptr_f !=NULL)?"(function: '"+ptr_f->name()+"')":
		(ptr_nt!=NULL)?"(nameTable: '"+ptr_nt->name()+"')":"(pointer: NULL)";
	default:
	  ERRP("unimplemented.");
	}
	return "#undef";
  }

}

#ifdef TEST
// test

using nl::Variable;

int main(){
  Variable a;
  a.dump();

  a.assign(10); // a = 10;
  a.dump();

  a.assign("Hello, world!!"); // a = 'Hello, world!!';
  a.dump();

  a.assign("10"); // a = '10';
  a.dump();
  
  a.assign(Variable::Integer, "10"); // a = (int)'10';
  a.dump();
  
  return 0;
}

#endif
