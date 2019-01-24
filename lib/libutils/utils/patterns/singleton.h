#include <iostream>

#ifndef UTILS_PATTERNS_SINGLETON_H_
#define UTILS_PATTERNS_SINGLETON_H_

namespace utils {
namespace patterns {

template<class TClass>
class Singleton {
public:
  virtual ~Singleton() {}

  static TClass& instance() {
    static TClass instance;
    return instance;
  }

protected:
  Singleton() {}
};

} // patterns
} // utils


#endif /* UTILS_PATTERNS_SINGLETON_H_ */
