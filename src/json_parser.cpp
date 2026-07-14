#include "json_parser.h"

#include <cctype>
#include <iostream>
#include <optional>

#include "json_tokeniser.h"
#include "json.h"

namespace json {

namespace __internal {

std::optional<Json> JsonParser::Parse() {
    const auto& value = this->value();

    if (!Consume(JsonTokeniser::TokenType::kEOF)) {
        return std::nullopt;
    }

    return value;
}

bool JsonParser::Match(const JsonTokeniser::TokenType& type) const {
    return current_token_.type == type;
}

bool JsonParser::Consume(const JsonTokeniser::TokenType& type) {
    if (current_token_.type == type) {
        current_token_ = tokeniser_.ReadNextToken();
        return true;
    }
    return false;
}

// objects
std::optional<Json> JsonParser::object() {
    if (!Consume(JsonTokeniser::TokenType::kLeftCurlyBracket)) {
        return std::nullopt;
    }

    auto&& object = Json::object();

    const auto& opt_key = raw_string();
    if (opt_key) {
        if (!Consume(JsonTokeniser::TokenType::kColon)) {
            return std::nullopt;
        }

        auto&& opt_value = this->value();
        if (!opt_value) {
            return std::nullopt;
        }

        object.put(opt_key.value(), std::move(*opt_value));

        while (Consume(JsonTokeniser::TokenType::kComma)) {
            const auto& opt_key = raw_string();
            if (!opt_key) {
                return std::nullopt;
            }

            if (!Consume(JsonTokeniser::TokenType::kColon)) {
                return std::nullopt;
            }

            auto&& opt_value = this->value();
            if (!opt_value) {
                return std::nullopt;
            }

            object.put(opt_key.value(), std::move(*opt_value));
        }
    }

    if (!Consume(JsonTokeniser::TokenType::kRightCurlyBracket)) {
        return std::nullopt;
    }

    return std::optional<Json>(object);
}

std::optional<Json> JsonParser::array() {
    if (!Consume(JsonTokeniser::TokenType::kLeftSquareBracket)) {
        return std::nullopt;
    }

    auto&& array = Json::array();
    std::optional<Json> opt_value = this->value();
    if (opt_value) {
        array.add(std::move(*opt_value));

        while (Consume(JsonTokeniser::TokenType::kComma)) {
            opt_value = this->value();

            if (!opt_value) {
                // No opt_value after coma.
                return std::nullopt;
            }

            array.add(std::move(*opt_value));
        }
    }

    if (!Consume(JsonTokeniser::TokenType::kRightSquareBracket)) {
        return std::nullopt;
    }

    return std::optional<Json>(array);
}

// base
std::optional<Json> JsonParser::value() {
    if (Match(JsonTokeniser::TokenType::kString)) {
        return string();
    } else if (Match(JsonTokeniser::TokenType::kNumber)) {
        return number();
    } else if (Match(JsonTokeniser::TokenType::kLeftCurlyBracket)) {
        return object();
    } else if (Match(JsonTokeniser::TokenType::kLeftSquareBracket)) {
        return array();
    } else if (Match(JsonTokeniser::TokenType::kTrue) || Match(JsonTokeniser::TokenType::kFalse)) {
        return boolean();
    } else if (Match(JsonTokeniser::TokenType::kNull)) {
        return null();
    }

    return std::nullopt;
}

// primitives
std::optional<Json> JsonParser::null() {
    if (Consume(JsonTokeniser::TokenType::kNull)) {
        return std::optional<Json>(Json::null());
    } else {
        return std::nullopt;
    }
}

std::optional<Json> JsonParser::boolean() {
    if (Consume(JsonTokeniser::TokenType::kTrue)) {
        return std::optional<Json>(Json(true));
    } else if (Consume(JsonTokeniser::TokenType::kFalse)) {
        return std::optional<Json>(Json(false));
    }

    return std::nullopt;
}

std::optional<Json> JsonParser::number() {
    const auto token = current_token_;

    if (!Consume(JsonTokeniser::TokenType::kNumber)) {
        return std::nullopt;
    }

    const auto value = tokeniser_.Extract(token);
    return std::optional<Json>(Json(std::stod(value)));
}

std::optional<Json> JsonParser::string() {
    const auto opt_string = raw_string();

    if (!opt_string) {
        return std::nullopt;
    }

    Json json(std::move(opt_string.value()));
    return std::optional<Json>(json);
}

std::optional<std::string> JsonParser::raw_string() {
    const auto token = current_token_;

    if (!Consume(JsonTokeniser::TokenType::kString)) {
        return std::nullopt;
    }

    const auto value = tokeniser_.Extract(token);
    return std::optional<std::string>(value.substr(1, value.length() - 2));
}

} // namespace internal

} // namespace json
