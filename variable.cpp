// created date : 2011/12/18 22:43:33
// last updated : 2011/12/29 15:40:29
// 動的型 dynamic type

#include "variable.h"

#include "util.h"

#include <cstdio> // for sprintf()
#include <cstdlib> // for atoi()
#include <iostream>
using namespace std;

namespace nl{
  static char buf_variable[1024];

  const int         Variable::undef_int = 0;
  const string Variable::undef_str = "#undef";

  // undef で初期化。vector<> のために public にする
  Variable::Variable()                          : type_(Undef  ), val_int(undef_int), val_str(undef_str), ptr_v(NULL),    ptr_f(NULL),    ptr_nt(NULL),     constant(false){}
  Variable::Variable(int val)                   : type_(Integer), val_int(val      ), val_str(undef_str), ptr_v(NULL),    ptr_f(NULL),    ptr_nt(NULL),     constant(false){}
  Variable::Variable(const string &val)         : type_(String ), val_int(undef_int), val_str(val      ), ptr_v(NULL),    ptr_f(NULL),    ptr_nt(NULL),     constant(false){}
  Variable::Variable(Variable *var)             : type_(Pointer), val_int(undef_int), val_str(undef_str), ptr_v(var ),    ptr_f(NULL),    ptr_nt(NULL),     constant(false){}		
  Variable::Variable(AbsFunction *func)         : type_(Pointer), val_int(undef_int), val_str(undef_str), ptr_v(NULL),    ptr_f(func),    ptr_nt(NULL),     constant(false){}
  Variable::Variable(Type type, const string &val) : type_(type), val_int(undef_int), val_str(undef_str), ptr_v(NULL),    ptr_f(NULL),    ptr_nt(NULL),     constant(false){ assign(type, val); }
  Variable::Variable(const Variable &o)         : type_(o.type_), val_int(o.val_int), val_str(o.val_str), ptr_v(o.ptr_v), ptr_f(o.ptr_f), ptr_nt(o.ptr_nt), constant(o.constant){}
  Variable &Variable::operator=(const Variable &obj){ return assign(obj); }

  /// assign()
  Variable &Variable::assign_undef()           { type_=Undef;   val_int = undef_int; val_str = undef_str; ptr_v = NULL; ptr_f = NULL; ptr_nt = NULL; return *this; }
  Variable &Variable::assign(int val)          { type_=Integer; val_int = val;       val_str = undef_str; ptr_v = NULL; ptr_f = NULL; ptr_nt = NULL; return *this; }
  Variable &Variable::assign(const string &val){ type_=String;  val_int = undef_int; val_str = val;       ptr_v = NULL; ptr_f = NULL; ptr_nt = NULL; return *this; }
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
	default:
	  ERRP("unimplemented.");
	}
	return undef_str;
  }
  
  /// 演算
  Variable  Variable::operator+ (const Variable& o){
	Type lt =   type_;
	Type rt = o.type_;
	// Undef -> 他方に合わせる
	if( lt == Undef ) lt = rt;
	else if( rt == Undef ) rt = lt;
	
	// 型が違う -> エラー
	if( lt != rt ) return ERRP("cannot add (" << dump_str() << ", " << o.dump_str() << ")"), undef();
	switch(lt){
	case Undef:   return undef();
	case Integer: return Variable( val_int + o.val_int );
	case String:  return Variable( val_str + o.val_str );
	case Pointer: ERRP("cannot add (" << dump_str() << ", " << dump_str() << ")"); break;
	default: ERRP("unimplemented.");
	}
	return undef();
  }
  Variable& Variable::operator+=(const Variable& o){
	Type lt =   type_;
	Type rt = o.type_;
	// Undef -> 他方に合わせる
	if( lt == Undef ) lt = rt;
	else if( rt == Undef ) rt = lt;
	
	// 型が違う -> エラー
	if( lt != rt ) return ERRP("cannot add (" << dump_str() << ", " << o.dump_str() << ")"), *this;
	switch(lt){
	case Undef:   break;
	case Integer: val_int += o.val_int; break;
	case String:  val_str += o.val_str; break;
	case Pointer: ERRP("cannot add (" << dump_str() << ", " << dump_str() << ")"); break;
	default: ERRP("unimplemented.");
	}
	return *this;
  }

 // return l+r;
  Variable Variable::add(const Variable &l, const Variable &r){
	Type lt = l.type_;
	Type rt = r.type_;
	// Undef -> 他方に合わせる
	if( lt == Undef ) lt = rt;
	else if( rt == Undef ) rt = lt;
	
	// 型が違う -> エラー
	if( lt != rt ) return ERRP("cannot add (" << l.dump_str() << ", " << r.dump_str() << ")"), undef();
	switch(lt){
	case Undef:   return undef();
	case Integer: return Variable( l.val_int + r.val_int );
	case String:  return Variable( l.val_str + r.val_str );
	case Pointer: ERRP("cannot add (" << l.dump_str() << ", " << r.dump_str() << ")"); break;
	default: ERRP("unimplemented.");
	}
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
