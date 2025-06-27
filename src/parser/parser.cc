#include "parser/parser.hpp"
#include <optional>

std::optional<Token> AST::peekToken()
{
    if (position + 1 >= tokens.size())
        return std::nullopt;
    return tokens[position + 1];
}

Token AST::currentToken() { return tokens[position]; }

std::optional<Token> AST::eatToken()
{
    if (position + 1 >= tokens.size())
        return std::nullopt;
    Token token = currentToken();
    position++;
    return token;
}

bool AST::matchToken(TokenKind kind)
{
    return currentToken().kind() == kind;
}
