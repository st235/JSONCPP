#ifndef JSONCPP_JSON_BEAUTIFIER_H_
#define JSONCPP_JSON_BEAUTIFIER_H_

#include <cstdint>
#include <sstream>

#include "json_visitor.h"

namespace json {

class JsonValue;

class JsonBeautifier: public JsonVisitor {
  public:
    JsonBeautifier(uint8_t spacing = 1,
                   char _space_symbol = '\t'):
        _depth(0),
        _spacing(spacing),
        _space_symbol(_space_symbol),
        _stream() {
        // Empty on purpose.
    }

    std::string beautify(const Json& json);

    virtual void visitNull() override;
    virtual void visitBoolean(bool_t node) override;
    virtual void visitNumber(number_t node) override;
    virtual void visitString(const string_t& node) override;
    virtual void visitArray(const array_t& node) override;
    virtual void visitObject(const object_t& node) override;

    virtual ~JsonBeautifier() = default;
  private:
    void writeSpacing() {
        for (uint16_t i = 0; i < _depth * _spacing; i++) {
            _stream << _space_symbol;
        }
    }

    void reset() {
        _depth = 0;
        _stream.str(std::string());
        _stream.clear();
    }

    inline std::string beautifiedJson() const {
        return _stream.str();
    }

    uint16_t _depth;
    uint8_t _spacing;
    char _space_symbol;
    std::stringstream _stream;
};

} // namespace json

#endif // JSONCPP_JSON_BEAUTIFIER_H_
