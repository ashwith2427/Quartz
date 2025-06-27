#include "lexer/lexer.hpp"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <format>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <stdexcept>

std::string readfile(std::string_view source)
{
    std::ifstream file(
        source.data(), std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open the file!");
    }
    std::size_t file_size = static_cast<std::size_t>(file.tellg());
    file.seekg(0);
    std::string result(file_size, '\0');
    file.read(result.data(), file_size);
    return result;
}

char Lexer::current() { return _source[_global_cursor]; }

std::optional<char> Lexer::next()
{
    if (_global_cursor + 1 >= _source.size()) {
        return std::nullopt;
    }
    return _source[_global_cursor + 1];
}

bool Lexer::match(std::string_view matcher)
{
    if (_global_cursor + matcher.size() >= _source.size()) {
        return false;
    }
    return _source.substr(_global_cursor).starts_with(matcher);
}

void Lexer::make_character_literal(std::vector<Token>& tokens)
{
    _global_cursor++;
    int temp = _global_cursor;
    std::string result = "";
    if (current() == '\\') {
        temp++;
        switch (_source[temp]) {
        case 'n':
            result += '\n';
            temp++;
            break;
        case 't':
            result += '\t';
            temp++;
            break;
        case 'b':
            result += '\b';
            temp++;
            break;
        case '\\':
            result += '\\';
            break;
        case 'r':
            result += '\r';
            temp++;
            break;
        case 'U': {
            while (_source[temp] != '\'') {
                if (temp >= _source.size()) {
                    throw std::runtime_error(
                        std::format("Unclosed character literal at "
                                    "line: {}, column: {}\n",
                            _line, _local_cursor));
                }
                result += _source[temp];
                temp++;
            }
            tokens.push_back(Token(TokenKind::UNICODE_LITERAL,
                Token::TokenLocation(
                    _global_cursor, _local_cursor, _line),
                result));
            _local_cursor += result.size() + 2;
            _global_cursor += temp;
            return;
        default:
            throw std::runtime_error(
                std::format("Unsupported character literal at line: "
                            "{}, column: {}\n",
                    _line, _local_cursor));
        }
        }
    } else {
        if (next() != '\'') {
            throw std::runtime_error(
                std::format("Unsupported character literal at line: "
                            "{}, column: {}\n",
                    _line, _local_cursor));
        }
        result += current();
    }
    tokens.push_back(Token(TokenKind::CHARACTER_LITERAL,
        Token::TokenLocation(_global_cursor, _local_cursor, _line),
        result));
    _global_cursor = temp + 1;
    _local_cursor += result.size() + 2;
}

void Lexer::make_string_literal(std::vector<Token>& tokens)
{
    _global_cursor++;
    int temp = 2;
    std::string result = "";
    while (current() != '"') {
        if (_global_cursor >= _source.size()) {
            throw std::runtime_error(std::format(
                "Unclosed String literal at line: {}, column: {}",
                _line, _local_cursor));
        }
        result += current();
        _global_cursor++;
        temp++;
    }
    tokens.push_back(Token(TokenKind::STRING_LITERAL,
        Token::TokenLocation(_global_cursor, _local_cursor, _line),
        result));
    _global_cursor++;
    _local_cursor += temp;
}

void Lexer::skip_white_spaces()
{
    while (_global_cursor < _source.size() and current() == ' ') {
        _global_cursor++;
        _local_cursor++;
    }
}

void Lexer::make_comment(std::vector<Token>& tokens, bool is_multi)
{
    _global_cursor += 2;
    int temp = 2;
    while (_global_cursor < _source.size()) {
        if (is_multi and current() == '\n') {
            _line++;
            _global_cursor++;
            _local_cursor = 0;
            temp = 0;
        }
        if (is_multi and match("*/")) {
            break;
        }

        if (!is_multi and current() == '\n') {
            break;
        }
        _global_cursor++;
        temp++;
    }
    tokens.push_back(Token(TokenKind::COMMENT,
        Token::TokenLocation(_global_cursor, _local_cursor, _line)));
    if (is_multi) {
        _global_cursor += 2;
        _local_cursor += temp;
        _local_cursor += 2;
    } else {
        _local_cursor += temp;
    }
}

std::string Lexer::make_identifier()
{
    int temp = _global_cursor;
    std::string result = "";
    while (temp < _source.size()
        and (isalnum(_source[temp]) or _source[temp] == '_')) {
        result += _source[temp];
        temp++;
    }
    return result;
}

bool Lexer::traverse_table(std::vector<Token>& tokens)
{
    for (const auto& other : p_table) {
        auto [op, kind] = other;
        if (match(op)) {
            tokens.push_back(Token(kind,
                Token::TokenLocation(
                    _global_cursor, _local_cursor, _line),
                op));
            _global_cursor += op.size();
            _local_cursor += op.size();
            return true;
        }
    }
    return false;
}

void Lexer::make_number(std::vector<Token>& tokens)
{
    bool is_decimal_set = false;
    std::string number = "";
    while (isnumber(current()) or current() == '.') {
        if (is_decimal_set and current() == '.') {
            throw std::runtime_error(std::format(
                "Malformed decimal number at line: {}, column: {}\n",
                _line, _local_cursor));
        }
        if (current() == '.') {
            is_decimal_set = true;
        }
        number += current();
        _global_cursor++;
    }
    if (is_decimal_set) {
        tokens.push_back(Token(TokenKind::FLOAT_LITERAL,
            Token::TokenLocation(
                _global_cursor, _local_cursor, _line),
            number));
    } else {
        tokens.push_back(Token(TokenKind::INTEGER_LITERAL,
            Token::TokenLocation(
                _global_cursor, _local_cursor, _line),
            number));
    }
    _local_cursor += number.size();
}

std::vector<Token> Lexer::scan()
{
    using TokenLocation = Token::TokenLocation;
    std::vector<Token> tokens;
    while (_global_cursor < _source.size()) {
        if (current() == ' ') {
            skip_white_spaces();
            continue;
        }
        if (isalpha(current()) or current() == '_') {
            std::string identifier = make_identifier();
            auto it = std::find(
                keywords.cbegin(), keywords.end(), identifier);
            if (it != keywords.cend()) {
                tokens.push_back(Token(TokenKind::KEYWORD,
                    TokenLocation(
                        _global_cursor, _local_cursor, _line),
                    identifier));
            } else {
                tokens.push_back(Token(TokenKind::IDENTIFIER,
                    TokenLocation(
                        _global_cursor, _local_cursor, _line),
                    identifier));
            }
            _global_cursor += identifier.size();
            _local_cursor += identifier.size();
            continue;
        }

        if (isnumber(current())) {
            make_number(tokens);
            continue;
        }

        if (current() == '\'') {
            make_character_literal(tokens);
            continue;
        }

        if (current() == '"') {
            make_string_literal(tokens);
            continue;
        }

        if (match("//")) {
            make_comment(tokens, false);
            continue;
        }
        if (match("/*")) {
            make_comment(tokens, true);
            continue;
        }

        if (current() == '\n') {
            _local_cursor = 0;
            _global_cursor++;
            _line++;
            continue;
        }

        if (traverse_table(tokens)) {
            continue;
        }

        throw std::runtime_error(
            std::format("Undefined token at line: {}, column: {}",
                _line, _local_cursor));
    }
    tokens.push_back(Token(TokenKind::END,
        TokenLocation(_global_cursor, _local_cursor, _line)));
    return tokens;
}
