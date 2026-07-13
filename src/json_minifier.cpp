#include "json_minifier.h"

#include "json.h"

namespace json {

std::string JsonMinifier::minify(const Json& json) {
    json.accept(*this);
    const auto& minified_json = minifiedJson();
    reset();
    return minified_json;
}

void JsonMinifier::visitNull() {
    _stream << "null";
}

void JsonMinifier::visitBoolean(bool_t node) {
    if (node) {
        _stream << "true";
    } else {
        _stream << "false";
    }
}

void JsonMinifier::visitNumber(number_t node) {
    _stream << node;
}

void JsonMinifier::visitString(const string_t& node) {
    _stream << '\"' << node << '\"';
}

void JsonMinifier::visitObject(const object_t& node) {
    _stream << '{';

    size_t i = 0;
    for (const auto& [key, child]: node) {
        if (i > 0) {
            _stream << ',';
        }

        _stream << '\"' << key << '\"' << ':';

        child.accept(*this);

        i += 1;
    }

    _stream << '}';
}

void JsonMinifier::visitArray(const array_t& node) {
    _stream << '[';

    size_t i = 0;
    for (const auto& child: node) {
        if (i > 0) {
            _stream << ',';
        }

        child.accept(*this);

        i += 1;
    }

    _stream << ']';
}

} // namespace json
