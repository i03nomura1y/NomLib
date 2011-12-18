// created date : 2011/12/18 22:43:33
// last updated : 2011/12/19 03:03:41
// 動的型 dynamic type

#include "variable.h"

#include "util.h"

#include <cstdio> // for sprintf()
#include <cstdlib> // for atoi()
#include <iostream>
using namespace std;

namespace nl{
  static char buf_valiable[1024];

  int         Variable::undef_int = 0;
  std::string Variable::undef_str = "#undef";

  Variable::Variable(Type type, const std::string &val)
	: type_(type), val_int(undef_int), val_str(undef_str){
	assign(type, val);
  }
  // val を指定したTypeに変換して代入
  void Variable::assign(Type type, const std::string &val){
	type_ = type;
	switch(type_){
	case undef:   break;
	case Integer: val_int = atoi(val.c_str()); break;
	case String:  val_str = val; break;
	default:
	  ERRP("unimplemented.");
	}
  }

  int Variable::asInt() const{
	switch(type_){
	case undef:   return undef_int;
	case Integer: return val_int;
	default:
	  ERRP("unimplemented.");
	}
	return undef_int;
  }
  std::string Variable::asStr() const{
	switch(type_){
	case undef: return undef_str;
	case Integer: snprintf(buf_valiable, 1023, "%d", val_int); return buf_valiable;
	case String: return val_str;
	default:
	  ERRP("unimplemented.");
	}
	return undef_str;
  }
  
  
  void Variable::dump(){
	cout << type_ << "|" << asStr() << endl;
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
