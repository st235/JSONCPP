#ifndef JSONCPP_JSON_ASSERT_H_
#define JSONCPP_JSON_ASSERT_H_

#ifdef ASSERT

#include <cassert>

#endif // ASSERT


#ifdef ASSERT

#define JASSERT(expression) assert(expression)
#define JTHROW() assert(false)
#define JUNREACHABLE() assert(false)

#else // !ASSERT

#define JASSERT(expression)
#define JTHROW()
#define JUNREACHABLE()

#endif // ASSERT

#endif // JSONCPP_JSON_ASSERT_H_
