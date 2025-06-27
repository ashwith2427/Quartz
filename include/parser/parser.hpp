#include "lexer/lexer.hpp"
#include <cstdint>
#include <span>

class AST {
private:
    std::uint32_t position;
    std::span<Token const> tokens;

public:
    AST(std::span<Token const> tokens)
        : tokens(tokens)
        , position(0)
    {
    }
    std::optional<Token> peekToken();
    Token currentToken();
    std::optional<Token> eatToken();
    bool matchToken(TokenKind kind);
    virtual void parse() = 0;
    virtual void print() = 0;
    virtual ~AST() = default;
};

class Parser : public AST {
public:
    Parser(std::span<Token const> tokens)
        : AST(tokens)
    {
    }
    void parse() override;
    void print() override;
};
