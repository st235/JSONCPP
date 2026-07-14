#ifndef JSONCPP_JSON_VISITOR_H_
#define JSONCPP_JSON_VISITOR_H_

#include "json_definitions.h"

namespace json {

class JsonVisitor {
  public:
    virtual void visitNull() = 0;
    virtual void visitBoolean(bool_t node) = 0;
    virtual void visitNumber(number_t node) = 0;
    virtual void visitString(const string_t& node) = 0;
    virtual void visitArray(const array_t& node) = 0;
    virtual void visitObject(const object_t& node) = 0;
    virtual ~JsonVisitor() = default;
};

} // namespace json

#endif // JSONCPP_JSON_VISITOR_H_
