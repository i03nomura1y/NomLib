// created date : 2011/12/18 22:43:33
// last updated : 2011/12/28 15:34:03
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
  const std::string Variable::undef_str = "#undef";

  Variable::Variable(Type type, const std::string &val)
	: type_(type), val_int(undef_int), val_str(undef_str), ptr_v(NULL), ptr_f(NULL), constant(false){
	assign(type, val);
  }
  // val を指定したTypeに変換して代入
  void Variable::assign(Type type, const std::string &val){
	type_ = type;
	switch(type_){
	case Undef:   assign_undef(); break;
	case Integer: assign(atoi(val.c_str())); break;
	case String:  assign(val); break;
	default:
	  ERRP("unimplemented.");
	}
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
  
  
  void Variable::dump() const{
	cout << dump_str() << endl;
  }
  std::string Variable::dump_str() const{
	switch(type_){
	case Undef: return "#undef";
	case Integer: snprintf(buf_variable, 1023, "%d", val_int); return "int|"+std::string(buf_variable);
	case String: return "string|"+val_str;
	case Pointer:
	  return
		(ptr_v!=NULL)?"(pointer: "+ptr_v->dump_str()+")":
		(ptr_f!=NULL)?"(function: '"+ptr_f->name()+"')":"(pointer: NULL)";
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
