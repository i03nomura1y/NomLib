#include "util.h"
namespace nl{
  // @return container に val が含まれれば true
  template <typename T>
  bool has(const std::set<T> &container, const T &val){
	return (container.find(val)!=container.end());
  }

  // std::string
  template <>
  bool has(const std::set<std::string> &container, const std::string &val){
	return (container.find(val)!=container.end());
  }
  
}; // namespace nl
