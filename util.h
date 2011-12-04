// -*- mode: cpp -*-
#ifndef __NOMLIB_UTIL_H__
#define __NOMLIB_UTIL_H__

#include <iostream>

#define DBGP(MSG) std::cout << __FILE__ << ":" << __LINE__ << ": dbg in " << __FUNCTION__ << "(): " << MSG << std::endl
#define ERRP(MSG) std::cerr << __FILE__ << ":" << __LINE__ << ": error in " << __FUNCTION__ << "(): " << MSG << std::endl

#endif
