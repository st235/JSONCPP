#ifndef __JSONC_JSON_PARSER_H__
#define __JSONC_JSON_PARSER_H__

#include <cstdint>
#include <optional>
#include <string>

namespace json {

class Json;

namespace __internal {

// Forward declaration of the reader.
class JsonTokeniser;

class JsonParser {
  public:
    std::optional<Json> parse(const std::string& raw_json);

  private:
    /**
     * Implemented with accordance
     * to https://www.json.org/json-en.html
     **/

    // Base.
    std::optional<Json> value(JsonTokeniser& reader);
    
    // Objects.
    std::optional<Json> object(JsonTokeniser& reader);
    std::optional<Json> array(JsonTokeniser& reader);

    // Primitives.
    std::optional<Json> null(JsonTokeniser& reader);
    std::optional<Json> boolean(JsonTokeniser& reader);
    std::optional<Json> number(JsonTokeniser& reader);
    std::optional<Json> string(JsonTokeniser& reader);

    // Misc.
    std::optional<std::string> raw_string(JsonTokeniser& reader);
    void whitespace(JsonTokeniser& reader);
};

} // namespace internal

} // namespace json

#endif // __JSONC_JSON_PARSER_H__
