// created date : 2011/12/18 22:43:33
// last updated : 2012/02/01 07:41:45
// 動的型 dynamic type

#include "variable.h"

#include "util.h"
#include "regex.h"

#include <cstdio> // for sprintf()
#include <cstdlib> // for atoi()
#include <iostream>
using namespace std;

namespace nl{
  static char buf_variable[1024];

  const long   Variable::undef_int = 0;
  const string Variable::undef_str = "#undef";

  const AbsFunction::Ptr  AbsFunction::NullPtr  = AbsFunction::Ptr();
  const AbsNameTable::Ptr AbsNameTable::NullPtr = AbsNameTable::Ptr();
  const Variable::Ptr     Variable::NullPtr     = Variable::Ptr();

  AbsNameTable::AbsNameTable() : parent_() { nl_INC(); }
  AbsNameTable::~AbsNameTable(){ nl_DEC(); }
  PtrV AbsNameTable::add (const Variable &name, PtrV var){
	switch( name.type() ){
	case Variable::String:  return add( name.asStr(), var );
	case Variable::Integer: return add( name.asInt(), var );
	default:
	  ERRP("unimplemented.");
	  return Variable::NullPtr;
	}
  }
  PtrV AbsNameTable::find(const Variable &name){
	switch( name.type() ){
	case Variable::String:  return find( name.asStr() );
	case Variable::Integer: return find( name.asInt() );
	default:
	  ERRP("unimplemented.");
	  return Variable::NullPtr;
	}
  }

  PtrNT AbsNameTable::push(Ptr parent, Ptr child){
	if( parent ) child->parent_ = parent;
	return child;
  }
  
  PtrNT AbsNameTable::pop(Ptr nt){
	if( !nt ) return nt;
	Ptr ret = nt->parent_;
	//nt->parent_.reset();
	return ret;
  }
  void AbsNameTable::dump_stack(){
	  DBGP(" TableName: '" << name() );
	  if(parent_) parent_->dump_stack();
	}


  Variable::~Variable(){ nl_DEC(); }

  // undef で初期化。vector<> のために public にする
  Variable::Variable()                : type_(Undef  ), val_int(undef_int), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const int    &v) : type_(Integer), val_int(v        ), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const long   &v) : type_(Integer), val_int(v        ), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const dbl    &v) : type_(Float  ), val_int(undef_int), val_dbl(v        ), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const bool   &v) : type_(Boolean), val_int(v?1:0    ), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const char   *v) : type_(String ), val_int(undef_int), val_dbl(undef_int), val_str(v        ), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const uchar  *v) : type_(String ), val_int(undef_int), val_dbl(undef_int), val_str((const char*)v ), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const string &v) : type_(String ), val_int(undef_int), val_dbl(undef_int), val_str(v        ), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const PtrV   &p) : type_(VoidPtr), val_int(undef_int), val_dbl(undef_int), val_str(undef_str), ptr_v(p), ptr_f( ), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const PtrF   &p) : type_(FuncPtr), val_int(undef_int), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f(p), ptr_nt( ), constant(false){ nl_INC(); }
  Variable::Variable(const PtrNT  &p) : type_(Array  ), val_int(undef_int), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt(p), constant(false){ nl_INC(); }
  Variable::Variable(const Variable &o) : type_(o.type_), val_int(o.val_int), val_dbl(o.val_dbl), val_str(o.val_str), ptr_v(o.ptr_v), ptr_f(o.ptr_f), ptr_nt(o.ptr_nt), constant(o.constant){ nl_INC(); }
  Variable::Variable(Type type, const string   &val) : type_(type),    val_int(undef_int), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC();  assign(type, val); }
  Variable::Variable(Type type, const Variable &val) : type_(type),    val_int(undef_int), val_dbl(undef_int), val_str(undef_str), ptr_v( ), ptr_f( ), ptr_nt( ), constant(false){ nl_INC();  assign(type, val); }

  /// assign()
  Variable &Variable::assign_undef()         { type_=Undef;   val_int = undef_int; val_dbl = undef_int; val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const int    &v){ type_=Integer; val_int = v;         val_dbl = undef_int; val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const long   &v){ type_=Integer; val_int = v;         val_dbl = undef_int; val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const dbl    &v){ type_=Integer; val_int = undef_int; val_dbl = v;         val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const bool   &v){ type_=Boolean; val_int = (v?1:0);   val_dbl = undef_int; val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const char   *v){ type_=String;  val_int = undef_int; val_dbl = undef_int; val_str = v;         ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const uchar  *v){ type_=String;  val_int = undef_int; val_dbl = undef_int; val_str = (const char*)v; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const string &v){ type_=String;  val_int = undef_int; val_dbl = undef_int; val_str = v;         ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = AbsNameTable::NullPtr; return *this; }
  Variable &Variable::assign(const PtrNT  &p){ type_=Array;   val_int = undef_int; val_dbl = undef_int; val_str = undef_str; ptr_v = Variable::NullPtr; ptr_f = AbsFunction::NullPtr; ptr_nt = p;                     return *this; }
  Variable &Variable::assign(const Variable &o){ type_=o.type_; val_int = o.val_int; val_dbl = o.val_dbl; val_str = o.val_str; ptr_v = o.ptr_v; ptr_f = o.ptr_f; ptr_nt = o.ptr_nt; constant = o.constant; return *this; }
  Variable &Variable::operator=(const Variable &obj){ return assign(obj); }
  
  Variable &Variable::assign(Type type, const string &val){  // val を指定したTypeに変換して代入
	type_ = type;
	switch(type_){
	case Undef  : return assign_undef();
	case Integer: return assign(atol(val.c_str()));
	case String : return assign(val);
	case Boolean: return assign(val == "true"  || val == "TRUE" ); // true, TRUE なら真
	case Auto   : return assign_auto(val);  // 自動判定
	default:
	  ERRP("unimplemented. " << dump_str());
	}
	return *this;
  }

  Variable &Variable::assign(Type type, const Variable    &val){ // val を指定したTypeに変換して代入
	type_ = type;
	switch(type_){
	case Undef:   assign_undef(); break;
	case Integer: assign( val.asInt()  ); break;
	case Boolean: assign( val.asBool() ); break;
	case String:  assign( val.asStr()  ); break;
	case Auto   : return assign_auto(val.asStr());  // 自動判定
	default:
	  ERRP("unimplemented. " << dump_str());
	}
	return *this;
  }

  // val の型を自動判別して代入
  Variable &Variable::assign_auto(const std::string &val){
	static RegEx rx_int("^[+-]?[0-9]+?$");
	static RegEx rx_dbl("^[+-]?[0-9]+([.][0-9]*)?$");
	if(val == "true"  || val == "TRUE" ) return assign(true);  // true,  TRUE  なら 真
	if(val == "false" || val == "FALSE") return assign(false); // false, FALSE なら 偽
	if( rx_int.match(val) ) return assign( atol(val.c_str()) ); // int正規表現にマッチ -> int
	//if( rx_dbl.match(val) ) return assign( atof(val.c_str()) ); // int正規表現にマッチ -> double
	return assign(val);
  }


  /// clone (Deep Copy)
  Variable::Ptr Variable::clone() const{
	Variable::Ptr ptr = Variable::Ptr(new Variable() );
	ptr->type_    = type_   ;
	ptr->val_int  = val_int ;
	ptr->val_str  = val_str ;
	ptr->constant = constant;
	ptr->ptr_f    = ptr_f   ; // Function はcloneしない
	if(ptr_v)  ptr->ptr_v   = ptr_v ->clone();
	if(ptr_nt) ptr->ptr_nt  = ptr_nt->clone();
	return ptr;
  }
	

  int Variable::asInt() const{
	switch(type_){
	case Undef  : return undef_int;
	case Integer: return val_int;
	case Boolean: return val_int;
	case String : return atol(val_str.c_str());
	default:
	  ERRP("unimplemented. " << dump_str());
	}
	return undef_int;
  }
  std::string Variable::asStr() const{
	switch(type_){
	case Undef: return undef_str;
	case Integer: snprintf(buf_variable, 1023, "%ld", (long)val_int); return buf_variable;
	case Boolean: return (val_int!=0)?"true":"false";
	case String:  return val_str;
	case FuncPtr: if(ptr_f) return ptr_f->name();
	default:
	  ERRP("unimplemented. " << dump_str());
	}
	return undef_str;
  }
  bool Variable::asBool() const{
	switch(type_){
	case Undef:   return false;
	case Integer: return (val_int!=0);
	case Boolean: return (val_int!=0);
	case String : return (val_str!="");
	case VoidPtr: return (ptr_v);
	case FuncPtr: return (ptr_f);
	case Array  : return (ptr_nt);
	default:
	  ERRP("uninplemented");
	}
	return false;
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
	  if( op == "-="){ return assign(val_int - o.val_int); }
	  if( op == "*="){ return assign(val_int * o.val_int); }
	  if( op == "/="){ return assign(val_int / o.val_int); }
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

  // 前置 単項演算子
  Variable Variable::asgn_oper(const std::string &op, bool except){
	switch( type_ ){
	case Undef:   return undef();
	case Integer:
	  if( op == "++"){ ++val_int; return *this; }
	  if( op == "--"){ --val_int; return *this; }
	  break;
	default:
	  break;
	}
	if(except) throw 0;
	ERRP("unimplemented operation '"<< op << "' (" << dump_str() << ")");
	return undef();
  }

  // 非代入系の演算
  Variable Variable::oper(const std::string &op, const Variable &o, bool except) const{
	
	if( op == "&&") return Variable( asBool() && o.asBool() );
	if( op == "||") return Variable( asBool() || o.asBool() );
	
	switch( fitType(type_, o.type_) ){
	case Undef:   return undef();
	case Integer:
	  if( op == "+" ) return Variable( val_int +  o.val_int );
	  if( op == "-" ) return Variable( val_int -  o.val_int );
	  if( op == "*" ) return Variable( val_int *  o.val_int );
	  if( op == "/" ) return Variable( val_int /  o.val_int );
	  if( op == "%" ) return Variable( val_int %  o.val_int );
	  if( op == "<" ) return Variable( val_int <  o.val_int );
	  if( op == "<=") return Variable( val_int <= o.val_int );
	  if( op == ">" ) return Variable( val_int >  o.val_int );
	  if( op == ">=") return Variable( val_int >= o.val_int );
	  if( op == "==") return Variable( val_int == o.val_int );
	  if( op == "!=") return Variable( val_int != o.val_int );
	  break;
	case Boolean:
	  if( op == "==") return Variable( val_int == o.val_int );
	  if( op == "!=") return Variable( val_int != o.val_int );
	case String:
	  if( op == "+" ) return Variable( val_str +  o.val_str );
	  if( op == "==") return Variable( val_str == o.val_str );
	  if( op == "!=") return Variable( val_str != o.val_str );
	  break;
	case TypeMissMatch:
	  if( op == "==") return Variable(false);
	  if( op == "!=") return Variable(true );

	  if( op == "<" ) return Variable( asInt() <  o.asInt() );
	  if( op == "<=") return Variable( asInt() <= o.asInt() );
	  if( op == ">" ) return Variable( asInt() >  o.asInt() );
	  if( op == ">=") return Variable( asInt() >= o.asInt() );
	  break;
	default:
	  break;
	}
	if(except) throw 0;
	ERRP("unimplemented operation '"<< op << "' (" << dump_str() << ", " << o.dump_str() << ")");
	return undef();
  }

  // 前置 単項演算子
  Variable Variable::oper(const std::string &op, bool except) const{
	if( op == "!") return Variable( !asBool() );

	switch( type_ ){
	case Undef:   return undef();
	case Integer:
	  if( op == "+" ) return *this;
	  if( op == "-" ) return Variable( - val_int );
	  break;
	default:
	  break;
	}
	if(except) throw 0;
	ERRP("unimplemented operation '"<< op << "' (" << dump_str() << ")");
	return undef();
  }


  void Variable::dump() const{
	cout << dump_str() << endl;
  }
  std::string Variable::dump_str() const{
	switch(type_){
	case Undef: return "#undef";
	case Integer: return "int|"   +asStr();
	case Boolean: return "bool|"  +asStr();
	case String : return "string|"+val_str;
	case VoidPtr: return "(pointer: "  +(ptr_v?(ptr_v->dump_str()):"NULL")+")";
	case FuncPtr: return "(function: " +(ptr_f?(ptr_f->name()):"NULL")+")";
	case Array  : return "(nameTable: "+(ptr_nt?(ptr_nt->name()):"NULL")+")";
	default:
	  ERRP("unimplemented. type_ : " << type_);
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
