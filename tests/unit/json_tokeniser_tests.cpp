#include <gtest/gtest.h>

#include <string>

#include "json_tokeniser.h"

using TokenType = json::__internal::JsonTokeniser::TokenType;
using TestArgs = std::pair<std::string, std::vector<TokenType>>;

class JsonTokeniserTestsFixture: public ::testing::TestWithParam<TestArgs> {};

INSTANTIATE_TEST_SUITE_P(
        JsonTokeniserTests,
        JsonTokeniserTestsFixture,
        ::testing::Values(
            // EOF.
            TestArgs("", { TokenType::kEOF }),
            TestArgs("          \t ", { TokenType::kEOF }),

            // Sanity checks: number.
            TestArgs("0", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("1", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("-0", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("-11", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("1234", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("444445555", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("-1.2", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("-1.2222222223", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("1112.2", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("1e10", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("10e2", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("2E4", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("10e-2", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("10e-1112", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("11.20e-1112", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("1.0e-1", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("1.0E+1", { TokenType::kNumber, TokenType::kEOF }),
            TestArgs("0.5", { TokenType::kNumber, TokenType::kEOF }),

            // Boundary checks: number.
            TestArgs("01", { TokenType::kError }),
            TestArgs("+5", { TokenType::kError }),

            // Sanity checks: string.
            TestArgs("\"\"", { TokenType::kString, TokenType::kEOF }),
            TestArgs("\"abcx\"", { TokenType::kString, TokenType::kEOF }),
            TestArgs("\"01 hello world\n \n abc \"", { TokenType::kString, TokenType::kEOF }),
            TestArgs("\"Привет мир\"", { TokenType::kString, TokenType::kEOF }),
            TestArgs("\"こんにちは、世界\"", { TokenType::kString, TokenType::kEOF }),

            // Boundary checks: string.
            TestArgs("\" abc d", { TokenType::kError }),

            // Tokens composition.
            TestArgs("{  \"a\" : 54}", { TokenType::kLeftCurlyBracket,
                TokenType::kString, TokenType::kColon, TokenType::kNumber,
                TokenType::kRightCurlyBracket, TokenType::kEOF }),
            TestArgs("{ \"field\" : [ true, null, \"abc\", 0.91 ] }", {
                TokenType::kLeftCurlyBracket, TokenType::kString, TokenType::kColon,
                TokenType::kLeftSquareBracket, TokenType::kTrue, TokenType::kComma,
                TokenType::kNull, TokenType::kComma, TokenType::kString,
                TokenType::kComma, TokenType::kNumber, TokenType::kRightSquareBracket,
                TokenType::kRightCurlyBracket, TokenType::kEOF }),

            // Misc.
            TestArgs("!", { TokenType::kError })
        )
);

TEST_P(JsonTokeniserTestsFixture, ValidJsonYieldsCorrectTokens) {
    const auto& pair = GetParam();

    const auto& json = pair.first;
    const auto& expected_token_types_sequence = pair.second;

    json::__internal::JsonTokeniser tokeniser(json);

    for (const auto& expected_token_type: expected_token_types_sequence) {
        const auto token = tokeniser.ReadNextToken();
        EXPECT_EQ(token.type, expected_token_type);
    }
}

TEST(JsonTokeniserTestsFixture, VerifyExtractWorksAsExpected) {
    json::__internal::JsonTokeniser tokeniser("true \"abc\" 5.2 \"Привет\"");

    auto token = tokeniser.ReadNextToken();
    ASSERT_EQ(token.type, TokenType::kTrue);
    ASSERT_EQ(tokeniser.Extract(token), std::string("true"));

    token = tokeniser.ReadNextToken();
    ASSERT_EQ(token.type, TokenType::kString);
    ASSERT_EQ(tokeniser.Extract(token), std::string("\"abc\""));

    token = tokeniser.ReadNextToken();
    ASSERT_EQ(token.type, TokenType::kNumber);
    ASSERT_EQ(tokeniser.Extract(token), std::string("5.2"));

    token = tokeniser.ReadNextToken();
    ASSERT_EQ(token.type, TokenType::kString);
    ASSERT_EQ(tokeniser.Extract(token), std::string("\"Привет\""));

    token = tokeniser.ReadNextToken();
    ASSERT_EQ(token.type, TokenType::kEOF);
}

