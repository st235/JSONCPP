#ifndef JSONCPP_JSON_MINIFIER_H_
#define JSONCPP_JSON_MINIFIER_H_

#include <sstream>

#include "json_visitor.h"

namespace json {

class Json;

class JsonMinifier: public JsonVisitor {
  public:
    JsonMinifier():
        _stream() {
        // Empty on purpose.
    }

    std::string minify(const Json& json);

    virtual void visitNull() override;
    virtual void visitBoolean(bool_t node) override;
    virtual void visitNumber(number_t node) override;
    virtual void visitString(const string_t& node) override;
    virtual void visitArray(const array_t& node) override;
    virtual void visitObject(const object_t& node) override;

    virtual ~JsonMinifier() = default;
  private:
    void reset() {
        _stream.str(std::string());
        _stream.clear();
    }

    inline std::string minifiedJson() const {
        return _stream.str();
    }

    std::stringstream _stream;
};

} // namespace json

#endif // JSONCPP_JSON_MINIFIER_H_
