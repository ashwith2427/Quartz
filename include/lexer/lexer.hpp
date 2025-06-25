#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace lexer {

enum TokenKind {
    Identifier,
    CharacterLiteral,
    StringLiteral,
    Integer,
    Character,
    Float,
    Double,
    Long,
    Number,
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    Comma,
    Dot,
    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,
    LeftShift,
    RightShift,
    BinaryOR,
    BinaryXOR,
    BinaryAND,
    Bang,
    LessThan,
    GreaterThan,
    BangEqual,
    LessThanEqual,
    GreaterThanEqual,
    EqualEqual,
    AssignEqual,
    AssignAdd,
    AssignSubtract,
    AssignMultiply,
    AssignDivide,
    AssignModulo,
    AssignLeftShift,
    AssignRightShift,
    AssignBinaryOR,
    AssignBinaryXOR,
    AssignBinaryAND,
    RelationalAND,
    RelationalOR,
    QuestionMark,
    Arrow,
    Semicolon,
    Colon,
    DoubleQuote,
    SingleQuote,
    MultiLineQuote,
    Tilda,
    Comment,
    MultilineComment,
    DocComment
};

struct TokenMeta {
    std::uint32_t column;
    std::uint32_t lineno;
    std::uint32_t start_pos;
    std::uint32_t size;
    TokenMeta(std::uint32_t column, std::uint32_t lineno,
        std::uint32_t start_pos, std::uint32_t size)
        : column(column)
        , lineno(lineno)
        , start_pos(start_pos)
        , size(size)
    {
    }
};

class Token {

public:
    Token(TokenKind kind, TokenMeta meta, std::string value)
        : kind_(kind)
        , meta_(meta)
        , lexeme_(value)
    {
    }
    Token(TokenKind kind, TokenMeta meta)
        : kind_(kind)
        , meta_(meta)
        , lexeme_(std::monostate {})
    {
    }
    inline bool has_value() const
    {
        return !std::holds_alternative<std::monostate>(lexeme_);
    }
    inline std::string_view lexeme() const
    {
        return std::get<std::string>(lexeme_);
    }
    inline TokenKind kind() const { return kind_; }
    inline TokenMeta meta() const { return meta_; }
    friend std::ostream& operator<<(
        std::ostream& os, Token const& tok)
    {
        if (tok.has_value()) {
            os << "Token: {\n";
            os << '\t' << "kind: " << tok.kind_ << '\n';
            os << '\t' << "location: {\n";
            os << '\t' << '\t' << "{ " << tok.meta_.lineno << ','
               << tok.meta_.start_pos << " }";
            os << '\t' << '}';
            os << '\t' << "Lexeme: " << tok.lexeme();
        } else {
            os << "Token: {\n";
            os << '\t' << "kind: " << tok.kind_ << '\n';
            os << '\t' << "location: {\n";
            os << '\t' << '\t' << "{ " << tok.meta_.lineno << ','
               << tok.meta_.start_pos << " }";
            os << '\t' << '}';
        }
        return os;
    }

private:
    TokenKind kind_;
    TokenMeta meta_;
    std::variant<std::monostate, std::string> lexeme_;
};

const std::unordered_map<char, TokenKind> singleCharacterTokens = {
    { '=', TokenKind::AssignEqual },
    { '!', TokenKind::Bang },
    { '~', TokenKind::Tilda },
    { '%', TokenKind::Modulo },
    { '^', TokenKind::BinaryXOR },
    { '&', TokenKind::BinaryAND },
    { '*', TokenKind::Multiply },
    { '(', TokenKind::LeftParen },
    { ')', TokenKind::RightParen },
    { '-', TokenKind::Minus },
    { '+', TokenKind::Plus },
    { '/', TokenKind::Divide },
    { '{', TokenKind::LeftBrace },
    { '}', TokenKind::RightBrace },
    { '[', TokenKind::LeftBracket },
    { ']', TokenKind::RightBracket },
    { ';', TokenKind::Semicolon },
    { ':', TokenKind::Colon },
    { ',', TokenKind::Comma },
    { '<', TokenKind::LessThan },
    { '.', TokenKind::Dot },
    { '>', TokenKind::GreaterThan },
    { '?', TokenKind::QuestionMark },
    { '|', TokenKind::BinaryOR },
};

const std::vector<char> escape_characters
    = { '\n', '\t', '\f', '\r', '\b' };

const std::unordered_map<std::string_view, TokenKind> builtin_types
    = {
          { "int", TokenKind::Integer },
          { "float", TokenKind::Float },
          { "char", TokenKind::Character },
          { "double", TokenKind::Double },
          { "long", TokenKind::Long },
      };

const std::unordered_map<std::string_view, TokenKind>
    doubleCharacterTokens = {
        { "!=", TokenKind::BangEqual },
        { "%=", TokenKind::AssignModulo },
        { "^=", TokenKind::AssignBinaryXOR },
        { "&=", TokenKind::AssignBinaryAND },
        { "*=", TokenKind::AssignMultiply },
        { "-=", TokenKind::AssignSubtract },
        { "+=", TokenKind::AssignAdd },
        { "<=", TokenKind::LessThanEqual },
        { ">=", TokenKind::GreaterThanEqual },
        { "/=", TokenKind::AssignDivide },
        { "==", TokenKind::EqualEqual },
        { "->", TokenKind::Arrow },
        { "<<", TokenKind::LeftShift },
        { ">>", TokenKind::RightShift },
    };

std::string read_file(std::string_view source);

class Lexer {
public:
    explicit Lexer(std::string_view source)
        : buffer_(read_file(source))
        , source_(buffer_)
        , offset_(0)
        , local_offset_(0)
        , lineno_(1)
    {
    }
    std::vector<Token> scan();

private:
    inline std::optional<char> next() const;
    inline bool match(std::string_view matcher) const;
    inline char current() const;
    void skip_white_spaces();
    std::string make_identifier();
    std::string make_number();
    std::string make_string_literal();
    std::string make_char_literal();
    std::string make_comment(bool multiline);
    void emit_token(std::vector<Token>& tokens, TokenKind kind,
        int len, std::optional<std::string> value);
    void eat();
    void eat(std::size_t len);
    std::string buffer_;
    std::string_view source_;
    std::uint32_t offset_;
    std::uint32_t lineno_;
    std::uint32_t local_offset_;
};
}
