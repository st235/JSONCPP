#ifndef JSONCPP_JSON_TOKENISER_H_
#define JSONCPP_JSON_TOKENISER_H_

#include <cctype>
#include <cstdint>
#include <string>

namespace json {

namespace __internal {

class JsonTokeniser {
  public:
    enum class TokenType {
        kEOF = 0,
        kError,
        kLeftCurlyBracket,
        kRightCurlyBracket,
        kLeftSquareBracket,
        kRightSquareBracket,
        kComma,
        kDot,
        kColon,
        kTrue,
        kFalse,
        kNull,
        kString,
        kNumber,
    };

    struct Token {
        TokenType type;
        size_t start;
        size_t length;
        const char* error_message;
    };

    explicit JsonTokeniser(const std::string& raw_json):
        start_index_(0),
        current_index_(0),
        raw_json_(raw_json) {
        // Empty on purpose.
    }

    JsonTokeniser(const JsonTokeniser& that) = default;
    JsonTokeniser& operator=(const JsonTokeniser& that) = default;
    JsonTokeniser(JsonTokeniser&& that) noexcept = default;
    JsonTokeniser& operator=(JsonTokeniser&& that) noexcept = default;

    std::string Extract(const Token& token) const {
        return raw_json_.substr(token.start, token.length);
    }

    Token ReadNextToken() {
        SkipWhitespace();

        if (current_index_ >= raw_json_.length()) {
            return MakeToken(TokenType::kEOF);
        }

        start_index_ = current_index_;

        char c = Advance();

        if (std::isdigit(c) || c == '-') {
            if (!ReadNumber(c == '0')) {
                return MakeErrorToken("Invalid number.");
            }

            return MakeToken(TokenType::kNumber);
        }

        switch (c) {
            case '.': return MakeToken(TokenType::kDot);
            case ',': return MakeToken(TokenType::kComma);
            case '{': return MakeToken(TokenType::kLeftCurlyBracket);
            case '}': return MakeToken(TokenType::kRightCurlyBracket);
            case '[': return MakeToken(TokenType::kLeftSquareBracket);
            case ']': return MakeToken(TokenType::kRightSquareBracket);
            case ':': return MakeToken(TokenType::kColon);
            case '\"': {
                if (!ReadString()) {
                    return MakeErrorToken("Invalid string literal.");
                }
                return MakeToken(TokenType::kString);
            }
            case 't': {
                if (!Match("rue")) {
                    return MakeErrorToken("Unknown token.");
                }
                return MakeToken(TokenType::kTrue);
            }
            case 'f': {
                if (!Match("alse")) {
                    return MakeErrorToken("Unknown token.");
                }
                return MakeToken(TokenType::kFalse);
            }
            case 'n': {
                if (!Match("ull")) {
                    return MakeErrorToken("Unknown token.");
                }
                return MakeToken(TokenType::kNull);
            }
        }

        return MakeErrorToken("Unknown token.");
    }

    ~JsonTokeniser() = default;

  private:
    size_t start_index_;
    size_t current_index_;
    std::string raw_json_;

    inline Token MakeToken(const TokenType& token_type) {
        JsonTokeniser::Token token;
        token.type = token_type;
        token.start = start_index_;
        token.length = current_index_ - start_index_;
        token.error_message = nullptr;
        return token;
    }

    inline Token MakeErrorToken(const char* message) {
        JsonTokeniser::Token token;
        token.type = JsonTokeniser::TokenType::kError;
        token.start = start_index_;
        token.length = 1;
        token.error_message = message;
        return token;
    }

    inline char Advance() {
        char c = Peek();
        if (current_index_ < raw_json_.length()) {
            current_index_++;
        }
        return c;
    }

    inline bool Consume(char c) {
        if (Peek() == c) {
            current_index_++;
            return true;
        }
        return false;
    }

    bool Match(const char* p) {
        while (*p != '\0') {
            if (!Consume(*p)) {
                return false;
            }
            p++;
        }
        return true;
    }

    char Peek() const {
        if (current_index_ >= raw_json_.length()) {
            return '\0';
        }
        return raw_json_[current_index_];
    }

    bool ReadString() {
        char c = Advance();
        while (c != '\0' && c != '\"') {
            if (c == '\\') {
                // Control character flow.
                c = Advance();
                bool is_escape_character =
                    (c == '\"') || (c == '\\') || (c == '/') ||
                    (c ==  'b') || (c ==  'f') || (c == 'n') ||
                    (c ==  'r') || (c ==  't');

                if (c == 'u') {
                    // 4 hex digits should follow.
                    c = Advance();

                    for (size_t i = 0; i < 4; i++) {
                        if (c == '\0' || !std::isxdigit(c)) {
                            return false;
                        }
                        c = Advance();
                    }
                } else if (is_escape_character) {
                    c = Advance();
                } else {
                    // Neither a control character nor escape symbol.
                    return false;
                }
            } else {
                c = Advance();
            }
        }

        if (c != '\"') {
            return false;
        }

        return true;
    }

    bool ReadNumber(bool hasLeadingZero) {
        // Should not have leading zeros.
        if (hasLeadingZero && std::isdigit(Peek())) {
            return false;
        }

        while (std::isdigit(Peek())) {
            Advance();
        }

        char c = Peek();
        if (c == '.') {
            Advance();

            // Fraction part.
            c = Peek();
            if (!std::isdigit(c)) {
                // Not a decimal after fraction.
                return false;
            }

            while (std::isdigit(Peek())) {
                Advance();
            }
        }

        c = Peek();
        if (c == 'e' || c == 'E') {
            Advance();

            // Exponent part.
            c = Peek();
            if (c == '-' || c == '+') {
                Advance();
            }

            c = Peek();
            if (!std::isdigit(c)) {
                // Not a decimal after exponent.
                return false;
            }

            while (std::isdigit(Peek())) {
                Advance();
            }
        }

        return true;
    }

    void SkipWhitespace() {
        // In terms of JSON specification
        // spaces are: space, linefeed (aka new line),
        // carriage return, and horizontal tab.
        while (Consume(' ') || Consume('\n') || Consume('\t') || Consume('\r')) {
        }
    }
};

} // namespace __internal

} // namespace json

#endif // JSONCPP_JSON_TOKENISER_H_