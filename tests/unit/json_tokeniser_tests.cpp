#include <gtest/gtest.h>

#include "json_tokeniser.h"

TEST(JsonTokeniser, CannotAdvanceInEmptyJson) {
    json::__internal::JsonTokeniser reader("");
    EXPECT_FALSE(reader.hasNext());
}

TEST(JsonTokeniser, CanAdvanceInNonEmptyJson) {
    json::__internal::JsonTokeniser reader("1");
    EXPECT_TRUE(reader.hasNext());
}

TEST(JsonTokeniser, ReachingEofReturnsFalse) {
    json::__internal::JsonTokeniser reader("1");

    EXPECT_TRUE(reader.next());
    EXPECT_FALSE(reader.next());
}

TEST(JsonTokeniser, PeekingValueReturnsCorrectCharacter) {
    json::__internal::JsonTokeniser reader("1a");
    EXPECT_EQ('1', reader.peek());
}

TEST(JsonTokeniser, PeekingAtEofReturnsTokenEOF) {
    json::__internal::JsonTokeniser reader("");
    EXPECT_EQ(json::__internal::JsonTokeniser::TOKEN_EOF, reader.peek());
}

TEST(JsonTokeniser, ConsumeReturnsTrueIfCharactersHaveMatched) {
    json::__internal::JsonTokeniser reader("abc");
    EXPECT_TRUE(reader.consume('a'));
}

TEST(JsonTokeniser, ConsumeReturnsFalseIfCharactersHaveNotMatched) {
    json::__internal::JsonTokeniser reader("abc");
    EXPECT_FALSE(reader.consume('b'));
}

TEST(JsonTokeniser, ConsumeReturnsFalseIfReachedEndOfTheStream) {
    json::__internal::JsonTokeniser reader("abc");

    reader.next();
    reader.next();
    reader.next();

    EXPECT_FALSE(reader.hasNext());
    EXPECT_FALSE(reader.consume('a'));
}

TEST(JsonTokeniser, ConsumeAllReturnsTrueIfTheEntirePatternMatched) {
    json::__internal::JsonTokeniser reader("spatterne");

    reader.next();

    EXPECT_TRUE(reader.consumeAll("pattern"));
}

TEST(JsonTokeniser, ConsumeAllReturnsFalseIfPatternHasNotMatched) {
    json::__internal::JsonTokeniser reader("spatterne");
    reader.next();

    EXPECT_FALSE(reader.consumeAll("abc"));
}

TEST(JsonTokeniser, ConsumeAllReturnsFalseIfPatternIsPartiallyMatched) {
    json::__internal::JsonTokeniser reader("spatterne");
    reader.next();

    EXPECT_FALSE(reader.consumeAll("patbcd"));
}

TEST(JsonTokeniser, ConsumeAllReturnsFalseIfReachedEndOfTheStream) {
    json::__internal::JsonTokeniser reader("spat");
    reader.next();

    EXPECT_FALSE(reader.consumeAll("pattern"));
}

TEST(JsonTokeniser, SaveAndRestoreReturnsReaderToInitialState) {
    json::__internal::JsonTokeniser reader("really long string");

    reader.next();
    reader.next();

    EXPECT_EQ('a', reader.peek());

    const auto& token = reader.save();

    reader.next();
    reader.next();
    reader.next();

    EXPECT_EQ('y', reader.peek());

    reader.restore(token);
    EXPECT_EQ('a', reader.peek());
}


