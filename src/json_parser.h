#ifndef JSONCPP_JSON_PARSER_H_
#define JSONCPP_JSON_PARSER_H_

#include <optional>
#include <string>

#include "json_tokeniser.h"

namespace json {

class Json;

namespace __internal {

class JsonParser {
  public:
    explicit JsonParser(const std::string& raw_json) noexcept:
        tokeniser_(raw_json),
        current_token_() {
        current_token_ = tokeniser_.ReadNextToken();
    }

    JsonParser(const JsonParser& that) noexcept = default;
    JsonParser& operator=(const JsonParser& that) noexcept = default;
    JsonParser(JsonParser&& that) noexcept = default;
    JsonParser& operator=(JsonParser&& that) noexcept = default;

    std::optional<Json> Parse();

    ~JsonParser() = default;

  private:
    JsonTokeniser tokeniser_;
    JsonTokeniser::Token current_token_;

    bool Match(const JsonTokeniser::TokenType& type) const;
    bool Consume(const JsonTokeniser::TokenType& type);

    /**
     * Implemented with accordance
     * to https://www.json.org/json-en.html
     **/

    // Base.
    std::optional<Json> value();
    
    // Objects.
    std::optional<Json> object();
    std::optional<Json> array();

    // Primitives.
    std::optional<Json> null();
    std::optional<Json> boolean();
    std::optional<Json> number();
    std::optional<Json> string();
    std::optional<std::string> raw_string();
};

} // namespace internal

} // namespace json

#endif // JSONCPP_JSON_PARSER_H_
