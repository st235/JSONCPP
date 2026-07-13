#include "json_beautifier.h"

#include "json.h"

namespace json {

std::string JsonBeautifier::beautify(const Json& json) {
    json.accept(*this);
    const auto& beautified_json = beautifiedJson();
    reset();
    return beautified_json;
}

void JsonBeautifier::visitNull() {
    _stream << "null";
}

void JsonBeautifier::visitBoolean(bool_t node) {
    if (node) {
        _stream << "true";
    } else {
        _stream << "false";
    }
}

void JsonBeautifier::visitNumber(number_t node) {
    _stream << node;
}

void JsonBeautifier::visitString(const string_t& node) {
    _stream << '\"' << node << '\"';
}

void JsonBeautifier::visitObject(const object_t& node) {
    _stream << '{' << std::endl;

    size_t i = 0;
    for (const auto& [key, child]: node) {
        _depth += 1;

        writeSpacing();
        _stream << '\"' << key << '\"' << ": ";

        child.accept(*this);

        if (i < (node.size() - 1)) {
            _stream << ',';
        }
        i += 1;

        _stream << std::endl;

        _depth -= 1;
    }

    writeSpacing();
    _stream << '}';
}

void JsonBeautifier::visitArray(const array_t& node) {
    _stream << '[' << std::endl;

    size_t i = 0;
    for (const auto& child: node) {
        _depth += 1;

        writeSpacing();
        child.accept(*this);

        if (i < (node.size() - 1)) {
            _stream << ',';
        }
        i += 1;

        _stream << std::endl;

        _depth -= 1;
    }

    writeSpacing();
    _stream << ']';
}

} // namespace json
