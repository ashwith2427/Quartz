#include "lexer/lexer.hpp"
#include <cassert>
#include <cctype>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace lexer {
std::string read_file(std::string_view source)
{
    std::ifstream file(source.data(), std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file\n");
    }
    std::size_t file_size = file.tellg();
    file.seekg(0);
    std::string result(file_size, '\0');
    file.read(result.data(), file_size);
    file.close();
    return result;
}

inline std::optional<char> Lexer::next() const
{
    if (offset_ + 1 >= source_.size())
        return std::nullopt;
    return source_[offset_ + 1];
}
inline bool Lexer::match(std::string_view matcher) const
{
    if (offset_ + matcher.size() >= source_.size()) {
        return false;
    }
    return source_.substr(offset_, matcher.size()) == matcher;
}
inline char Lexer::current() const { return source_[offset_]; }
void Lexer::skip_white_spaces()
{
    while (offset_ < source_.size() and source_[offset_] == ' ') {
        eat();
    }
}
std::string Lexer::make_identifier()
{
    std::string result = "";
    while (offset_ < source_.size() and isalnum(source_[offset_])) {
        result += source_[offset_];
        offset_++;
    }
    return result;
}

std::string Lexer::make_number()
{
    std::string result = "";
    while (offset_ < source_.size() and isdigit(source_[offset_])) {
        result += source_[offset_];
        offset_++;
    }
    return result;
}
std::string Lexer::make_string_literal()
{
    offset_++;
    std::string result = "";
    while (current() != '"') {
        if (offset_ >= source_.size()) {
            throw std::runtime_error(
                std::format("No Closing tag for string literal at "
                            "Line: {}, Column: {}",
                    lineno_, local_offset_));
        }
        result += current();
        offset_++;
    }
    offset_++;
    return result;
}

std::string Lexer::make_char_literal()
{
    std::string result = "";
    offset_++;
    char ch;
    if (current() == '\\') {
        offset_++;
        switch (current()) {
        case 'n':
            ch = '\n';
            break;
        case 't':
            ch = '\t';
            break;
        case 'r':
            ch = '\r';
            break;
        case 'b':
            ch = '\b';
            break;
        case '\\':
            ch = '\\';
            break;
        case '\'':
            ch = '\'';
            break;
        default:
            throw std::runtime_error(std::format(
                "Unsupported character at Line: {}, Column: {}",
                lineno_, local_offset_));
        }
    } else {
        ch = current();
    }
    offset_++;
    result += ch;
    return result;
}

std::string Lexer::make_comment(bool multiline)
{
    std::string result = "";
    offset_ += 2;
    while (offset_ < source_.size()) {
        if (!multiline and current() == '\n') {
            break;
        }
        if (multiline and current() == '\n') {
            lineno_++;
        }
        if (multiline and match("*/")) {
            offset_ += 2;
            break;
        }
        result += current();
        offset_++;
    }
    return result;
}

void Lexer::emit_token(std::vector<Token>& tokens, TokenKind kind,
    int len, std::optional<std::string> value = std::nullopt)
{
    if (value.has_value()) {
        tokens.push_back(Token(kind,
            TokenMeta(offset_, lineno_, local_offset_, len),
            value.value()));
    } else {
        tokens.push_back(Token(
            kind, TokenMeta(offset_, lineno_, local_offset_, len)));
    }
}

std::vector<Token> Lexer::scan()
{
    std::vector<Token> tokens;
    while (offset_ < source_.size()) {
        if (current() == ' ') {
            skip_white_spaces();
        }
        if (isalpha(current())) {
            std::string identifier = make_identifier();
            auto it = builtin_types.find(identifier);
            if (it != builtin_types.end()) {
                emit_token(tokens, it->second, identifier.size());
            } else {
                emit_token(tokens, TokenKind::Identifier,
                    identifier.size(), identifier);
            }
            local_offset_ += identifier.size();
            continue;
        }
        if (isdigit(current())) {
            std::string number = make_number();
            emit_token(
                tokens, TokenKind::Number, number.size(), number);
            local_offset_ += number.size();
            continue;
        }
        if (current() == '\'') {
            std::string char_literal = make_char_literal();
            local_offset_ += char_literal.size() + 2;
            emit_token(tokens, TokenKind::CharacterLiteral,
                char_literal.size(), char_literal);
            continue;
        }
        if (current() == '"') {
            std::string string_literal = make_string_literal();
            emit_token(tokens, TokenKind::StringLiteral,
                string_literal.size(), string_literal);
            local_offset_ += string_literal.size() + 2;
            continue;
        }
        if (match("//") or match("/*")) {
            bool multiline = match("/*");
            std::string comment = make_comment(multiline);
            if (multiline) {
                emit_token(tokens, TokenKind::MultilineComment,
                    comment.size() + 4, comment);
                local_offset_ += comment.size() + 4;
            } else {
                emit_token(tokens, TokenKind::Comment,
                    comment.size() + 2, comment);
                local_offset_ += comment.size() + 2;
            }
            continue;
        }
        auto dt_t
            = doubleCharacterTokens.find(source_.substr(offset_, 2));
        if (dt_t != doubleCharacterTokens.end()) {
            std::string_view val = dt_t->first;
            TokenKind kind = dt_t->second;
            emit_token(tokens, kind, val.size());
            eat(2);
            continue;
        }
        auto st_t = singleCharacterTokens.find(current());
        if (st_t != singleCharacterTokens.end()) {
            char val = st_t->first;
            TokenKind kind = st_t->second;
            emit_token(tokens, kind, 1);
            eat();
            continue;
        }
        if (current() == '\n') {
            local_offset_ = 0;
            offset_++;
            lineno_++;
        } else {
            std::cout << source_[offset_] << '\n';
            throw std::runtime_error(std::format(
                "Undefined Symbol at Line: {}, Column: {}\n", lineno_,
                local_offset_));
        }
    }
    return tokens;
}

void Lexer::eat()
{
    if (offset_ + 1 >= source_.size()) {
        throw std::runtime_error("Cannot eat!\n");
    }
    offset_++;
    local_offset_++;
}
void Lexer::eat(std::size_t len)
{
    if (offset_ + len >= source_.size()) {
        throw std::runtime_error("Cannot eat!\n");
    }
    offset_ += len;
    local_offset_ += len;
}
}
