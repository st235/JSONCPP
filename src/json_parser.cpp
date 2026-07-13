#include "json_parser.h"

#include <cctype>
#include <iostream>
#include <optional>
#include <sstream>

#include "json_token_reader.h"

#include "json.h"

namespace {

bool IsEscapeCharacter(char c) {
    return (c == '\"') || (c == '\\') || (c == '/') ||
        (c == 'b') || (c == 'f') || (c == 'n') ||
        (c == 'r') || (c == 't');
}

} // namespace

namespace json {

namespace __internal {

std::optional<Json> JsonParser::parse(const std::string& raw_json) {
    JsonTokenReader reader(raw_json);
    const auto& value = this->value(reader);

    if (reader.hasNext()) {
        return std::nullopt;
    }

    return value;
}

// objects
std::optional<Json> JsonParser::object(JsonTokenReader& reader) {
    auto token = reader.save();

    if (!reader.consume('{')) {
        reader.restore(token);
        return std::nullopt;
    }

    auto&& object = Json::object();

    whitespace(reader);

    const auto& opt_key = raw_string(reader);
    if (opt_key) {
        whitespace(reader);

        if (!reader.consume(':')) {
            reader.restore(token);
            return std::nullopt;
        }

        auto&& opt_value = this->value(reader);
        if (!opt_value) {
            reader.restore(token);
            return std::nullopt;
        }

        object.put(opt_key.value(), std::move(*opt_value));

        while (reader.consume(',')) {
            whitespace(reader);
            const auto& opt_key = raw_string(reader);
            if (!opt_key) {
                reader.restore(token);
                return std::nullopt;
            }

            whitespace(reader);

            if (!reader.consume(':')) {
                reader.restore(token);
                return std::nullopt;
            }

            auto&& opt_value = this->value(reader);
            if (!opt_value) {
                reader.restore(token);
                return std::nullopt;
            }

            object.put(opt_key.value(), std::move(*opt_value));
        }
    }

    if (!reader.consume('}')) {
        reader.restore(token);
        return std::nullopt;
    }

    return std::optional<Json>(object);
}

std::optional<Json> JsonParser::array(JsonTokenReader& reader) {
    auto token = reader.save();

    if (!reader.consume('[')) {
        reader.restore(token);
        return std::nullopt;
    }

    auto&& array = Json::array();
    std::optional<Json> opt_value = this->value(reader);
    if (opt_value) {
        array.add(std::move(*opt_value));

        while (reader.consume(',')) {
            opt_value = this->value(reader);

            if (!opt_value) {
                // No opt_value after coma.
                reader.restore(token);
                return std::nullopt;
            }

            array.add(std::move(*opt_value));
        }
    }

    whitespace(reader);

    if (!reader.consume(']')) {
        reader.restore(token);
        return std::nullopt;
    }

    return std::optional<Json>(array);
}

// base
std::optional<Json> JsonParser::value(JsonTokenReader& reader) {
    auto token = reader.save();

    std::optional<Json> value = std::nullopt;
    whitespace(reader);

    value = string(reader);

    if (!value) {
        value = number(reader);
    }

    if (!value) {
        value = object(reader);
    }

    if (!value) {
        value = array(reader);
    }

    if (!value) {
        value = boolean(reader);
    }

    if (!value) {
        value = null(reader);
    }

    if (!value) {
        reader.restore(token);
        return std::nullopt;
    }

    whitespace(reader);

    return value;
}

// primitives
std::optional<Json> JsonParser::null(JsonTokenReader& reader) {
    auto token = reader.save();

    if (reader.consumeAll("null")) {
        return std::optional<Json>(Json::null());
    } else {
        reader.restore(token);
        return std::nullopt;
    }
}

std::optional<Json> JsonParser::boolean(JsonTokenReader& reader) {
    auto token = reader.save();

    if (reader.consumeAll("true")) {
        return std::optional<Json>(Json(true));
    } else {
        reader.restore(token);
    }

    if (reader.consumeAll("false")) {
        return std::optional<Json>(Json(false));
    } else {
        reader.restore(token);
    }

    return std::nullopt;
}

std::optional<Json> JsonParser::number(JsonTokenReader& reader) {
    auto token = reader.save();

    std::stringstream sstream;

    if (reader.peek() == '-') {
        sstream << reader.next();
    }

    if (std::isdigit(reader.peek())) {
        bool isZero = (reader.peek() == '0');
        sstream << reader.next();

        if (!isZero) {
            // Digit 1-9 (as number should not have leading zeros)
            while (std::isdigit(reader.peek())) {
                sstream << reader.next();
            }
        }
    } else {
        // Not a decimal.
        reader.restore(token);
        return std::nullopt;
    }

    if (reader.peek() == '.') {
        // Fraction part.
        sstream << reader.next();

        if (!std::isdigit(reader.peek())) {
            // Not a decimal after fraction.
            reader.restore(token);
            return std::nullopt;
        }

        while (std::isdigit(reader.peek())) {
            sstream << reader.next();
        }
    }

    if (reader.peek() == 'e' || reader.peek() == 'E') {
        // Exponent part.
        sstream << reader.next();

        if (reader.peek() == '-' || reader.peek() == '+') {
            sstream << reader.next();
        }

        if (!std::isdigit(reader.peek())) {
            // Not a decimal after exponent.
            reader.restore(token);
            return std::nullopt;
        }

        while (std::isdigit(reader.peek())) {
            sstream << reader.next();
        }
    }

    return std::optional<Json>(Json(std::stod(sstream.str())));
}

std::optional<Json> JsonParser::string(JsonTokenReader& reader) {
    const auto& opt_string = this->raw_string(reader);
    if (!opt_string) {
        return std::nullopt;
    }

    Json json(std::move(opt_string.value()));
    return std::optional<Json>(json);
}

// misc
std::optional<std::string> JsonParser::raw_string(JsonTokenReader& reader) {
    auto token = reader.save();

    if (!reader.consume('\"')) {
        return std::nullopt;
    }

    std::stringstream sstream;

    while (reader.hasNext() && reader.peek() != '\"') {
        char c = reader.next();
        sstream << c;

        if (c == '\\') {
            // Control character flow.

            if (!reader.hasNext()) {
                reader.restore(token);
                return std::nullopt;
            }

            char control_character = reader.next();
            sstream << control_character;

            if (control_character == 'u') {
                // 4 hex digits should follow

                for (size_t i = 0; i < 4; i++) {
                    if (!reader.hasNext() || !std::isxdigit(reader.peek())) {
                        reader.restore(token);
                        return std::nullopt;
                    }

                    sstream << reader.next();
                }
            } else if (!IsEscapeCharacter(control_character)) {
                // Neither a control character nor hexadecimal number.
                reader.restore(token);
                return std::nullopt;
            }
        }
    }

    if (reader.consume('\"')) {
        return std::optional<std::string>(sstream.str());
    }

    reader.restore(token);
    return std::nullopt;
}

void JsonParser::whitespace(JsonTokenReader& reader) {
    // In terms of JSON specification
    // spaces are: space, linefeed (aka new line),
    // carriage return, and horizontal tab.
    while (reader.consume(' ') || reader.consume('\n') || reader.consume('\t') || reader.consume('\r')) {
    }
}

} // namespace internal

} // namespace json
