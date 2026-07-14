#include <gtest/gtest.h>

#include <string>
#include <optional>
#include <utility>

#include "json.h"

class MalformedJsonTestingFixture: public ::testing::TestWithParam<std::string> {};

INSTANTIATE_TEST_SUITE_P(
        MalformedJsonTests,
        MalformedJsonTestingFixture,
        ::testing::Values(
            // Empty strings.
            "",
            "\t     \n     ",
            "\r\t\n      \t\t\r\r\n\n",

            // Null with typos.
            "nulls",

            // Booleans with typos.
            "truth",
            "falso",

            // Numbers with issues:
            // Leading zero.
            "0111",
            "025.56",
            // Fraction with no digits.
            "111.",
            "52.",
            // Fraction with coma separator.
            "5,22",
            "1,07",
            // Exponent with no digits.
            "1e",
            "-5e",
            "4E+",
            "1.23E",
            "-8.11e-",
            // Plus sign.
            "+3",
            "+3.11",
            "+2e3",
            // Multiple decimal dot separation.
            "-1.1.2",
            "1.23.11",
            // Mixed numbers and strings.
            "1.2a",
            "1.a",
            "22a",
            "1a2",
            "33ea",

            // Strings with issues:
            // No brackets.
            "hello world",
            // No matching closing bracket.
            R"("hello world)",
            R"(hello world")",
            // Disallowed character after control symbol.
            R"("hello \z ")",
            R"(" \a ")",
            // Invalid length of hex number.
            R"("\u01A")",
            R"("\uF0")",
            // Invalid hex number.
            R"("\uXAF1")",
            R"("number is \u1Z11")"

            // Arrays with issues:
            // No closing bracket.
            "["
            // No value.
            "[,]"
            // Trailing coma.
            "[5,]",
            "[true, {}, ]",

            // Objects with issues:
            // No closing bracket.
            "{",
            // Traling coma.
            R"({ "a" : 5, })",
            R"({ "a" : 5, "b": true, })",

            // Complex cases:
            // Multiple objects without array brackets.
            "5, 2, 1, true",
            "{}, {}",
            // Unknown keyword.
            R"({ "a" : hello_world })"
        )
);

TEST_P(MalformedJsonTestingFixture, MalformedJsonYieldsNull) {
    const auto& json = GetParam();
    const auto& value = json::Json::fromJson(json);
    EXPECT_EQ(std::nullopt, value);
}
