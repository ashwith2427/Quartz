#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

constexpr std::array<std::string, 20> keywords = {
    "uint8",
    "uint16",
    "uint32",
    "uint64",
    "int16",
    "int32",
    "int64",
    "void",
    "for",
    "while",
    "if",
    "else if",
    "else",
    "return",
    "break",
    "continue",
    "switch",
    "import",
    "struct",
    "enum",
};

enum TokenKind {
    KEYWORD = 0,
    IDENTIFIER,
    ARROW,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    CHARACTER_LITERAL,
    UNICODE_LITERAL,
    OP_ADD,
    OP_SUB,
    OP_DIV,
    OP_MUL,
    OP_MOD,
    OP_LEFT_SHIFT,
    OP_RIGHT_SHIFT,
    BINOP_AND,
    BINOP_NOR,
    BINOP_XOR,
    BINOP_OR,
    ASSIGN_OP_ADD,
    ASSIGN_OP_SUB,
    ASSIGN_OP_DIV,
    ASSIGN_OP_MUL,
    ASSIGN_OP_MOD,
    ASSIGN,
    ASSIGN_LEFT_SHIFT,
    ASSIGN_RIGHT_SHIFT,
    ASSIGN_BINOP_OR,
    ASSIGN_BINOP_XOR,
    ASSIGN_BINOP_AND,
    REL_OP_GREATER_THAN,
    REL_OP_LESS_THAN,
    REL_OP_GREATER_THAN_EQUAL,
    REL_OP_LESS_THAN_EQUAL,
    REL_OP_EQUAL,
    REL_OP_NOT_EQUAL,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_PAREN,
    CLOSE_PAREN,
    DOT,
    COMMA,
    QUESTION_MARK,
    COLON,
    COMMENT,
    INVALID,
    END,
};

inline const char* TokenKindToString(TokenKind kind)
{
    switch (kind) {
    case KEYWORD:
        return "KEYWORD";
    case INTEGER_LITERAL:
        return "INTEGER_LITERAL";
    case FLOAT_LITERAL:
        return "FLOAT_LITERAL";
    case IDENTIFIER:
        return "IDENTIFIER";
    case STRING_LITERAL:
        return "STRING_LITERAL";
    case CHARACTER_LITERAL:
        return "CHARACTER_LITERAL";
    case UNICODE_LITERAL:
        return "UNICODE_LITERAL";
    case OP_ADD:
        return "OP_ADD";
    case OP_SUB:
        return "OP_SUB";
    case OP_DIV:
        return "OP_DIV";
    case OP_MUL:
        return "OP_MUL";
    case OP_MOD:
        return "OP_MOD";
    case OP_LEFT_SHIFT:
        return "OP_LEFT_SHIFT";
    case OP_RIGHT_SHIFT:
        return "OP_RIGHT_SHIFT";
    case BINOP_AND:
        return "BINOP_AND";
    case BINOP_NOR:
        return "BINOP_NOR";
    case BINOP_XOR:
        return "BINOP_XOR";
    case BINOP_OR:
        return "BINOP_OR";
    case ASSIGN_OP_ADD:
        return "ASSIGN_OP_ADD";
    case ASSIGN_OP_SUB:
        return "ASSIGN_OP_SUB";
    case ASSIGN_OP_DIV:
        return "ASSIGN_OP_DIV";
    case ASSIGN_OP_MUL:
        return "ASSIGN_OP_MUL";
    case ASSIGN_OP_MOD:
        return "ASSIGN_OP_MOD";
    case ASSIGN:
        return "ASSIGN";
    case ASSIGN_LEFT_SHIFT:
        return "ASSIGN_LEFT_SHIFT";
    case ASSIGN_RIGHT_SHIFT:
        return "ASSIGN_RIGHT_SHIFT";
    case ASSIGN_BINOP_OR:
        return "ASSIGN_BINOP_OR";
    case ASSIGN_BINOP_XOR:
        return "ASSIGN_BINOP_XOR";
    case ASSIGN_BINOP_AND:
        return "ASSIGN_BINOP_AND";
    case REL_OP_GREATER_THAN:
        return "REL_OP_GREATER_THAN";
    case REL_OP_LESS_THAN:
        return "REL_OP_LESS_THAN";
    case REL_OP_GREATER_THAN_EQUAL:
        return "REL_OP_GREATER_THAN_EQUAL";
    case REL_OP_LESS_THAN_EQUAL:
        return "REL_OP_LESS_THAN_EQUAL";
    case REL_OP_EQUAL:
        return "REL_OP_EQUAL";
    case REL_OP_NOT_EQUAL:
        return "REL_OP_NOT_EQUAL";
    case OPEN_BRACE:
        return "OPEN_BRACE";
    case CLOSE_BRACE:
        return "CLOSE_BRACE";
    case OPEN_BRACKET:
        return "OPEN_BRACKET";
    case CLOSE_BRACKET:
        return "CLOSE_BRACKET";
    case OPEN_PAREN:
        return "OPEN_PAREN";
    case CLOSE_PAREN:
        return "CLOSE_PAREN";
    case DOT:
        return "DOT";
    case COMMA:
        return "COMMA";
    case QUESTION_MARK:
        return "QUESTION_MARK";
    case COLON:
        return "COLON";
    case COMMENT:
        return "COMMENT";
    case ARROW:
        return "ARROW";
    case INVALID:
        return "INVALID";
    default:
        return "UNKNOWN";
    }
}

const std::vector<std::pair<std::string, TokenKind>> p_table = {
    { ">>=", TokenKind::ASSIGN_RIGHT_SHIFT },
    { "<<=", TokenKind::ASSIGN_LEFT_SHIFT },
    { "==", TokenKind::REL_OP_EQUAL },
    { ">=", TokenKind::REL_OP_GREATER_THAN_EQUAL },
    { "<=", TokenKind::REL_OP_LESS_THAN_EQUAL },
    { "!=", TokenKind::REL_OP_NOT_EQUAL },
    { "+=", TokenKind::ASSIGN_OP_ADD },
    { "-=", TokenKind::ASSIGN_OP_SUB },
    { "/=", TokenKind::ASSIGN_OP_DIV },
    { "*=", TokenKind::ASSIGN_OP_MUL },
    { "%=", TokenKind::ASSIGN_OP_MOD },
    { "^=", TokenKind::ASSIGN_BINOP_XOR },
    { "&=", TokenKind::ASSIGN_BINOP_AND },
    { "|=", TokenKind::ASSIGN_BINOP_OR },
    { ">>", TokenKind::OP_RIGHT_SHIFT },
    { "<<", TokenKind::OP_LEFT_SHIFT },
    { "->", TokenKind::ARROW },
    { "+", TokenKind::OP_ADD },
    { "-", TokenKind::OP_SUB },
    { "/", TokenKind::OP_DIV },
    { "*", TokenKind::OP_MUL },
    { "%", TokenKind::OP_MOD },
    { "^", TokenKind::BINOP_XOR },
    { "&", TokenKind::BINOP_AND },
    { "=", TokenKind::ASSIGN },
    { "|", TokenKind::BINOP_OR },
    { "!", TokenKind::BINOP_NOR },
    { ">", TokenKind::REL_OP_GREATER_THAN },
    { "<", TokenKind::REL_OP_LESS_THAN },
    { ".", TokenKind::DOT },
    { ",", TokenKind::COMMA },
    { ":", TokenKind::COLON },
    { "?", TokenKind::QUESTION_MARK },
    { "(", TokenKind::OPEN_PAREN },
    { ")", TokenKind::CLOSE_PAREN },
    { "[", TokenKind::OPEN_BRACKET },
    { "]", TokenKind::CLOSE_BRACKET },
    { "{", TokenKind::OPEN_BRACE },
    { "}", TokenKind::CLOSE_BRACE },
};

class Token {
public:
    struct TokenLocation {
        std::uint32_t _offset;
        std::uint32_t _column;
        std::uint32_t _lineno;
    };
    Token(TokenKind const& kind, TokenLocation const& location,
        std::optional<std::string_view> lexeme = std::nullopt)
        : _kind(kind)
        , _location(location)
        , _lexeme(lexeme)
    {
    }
    inline TokenKind kind() const { return _kind; }
    inline bool has_value() const { return _lexeme.has_value(); }
    inline std::string_view value() const { return _lexeme.value(); }
    inline TokenLocation location() const { return _location; }

private:
    TokenKind _kind;
    std::optional<std::string_view> _lexeme;
    TokenLocation _location;
};

std::string readfile(std::string_view);

class Lexer {
public:
    Lexer(std::string_view source)
        : _source(readfile(source))
        , _global_cursor(0)
        , _local_cursor(0)
        , _line(1)
    {
    }

    std::vector<Token> scan();

private:
    char current();
    std::optional<char> next();
    bool match(std::string_view matcher);
    void make_character_literal(std::vector<Token>& tokens);
    void make_string_literal(std::vector<Token>& tokens);
    void make_comment(std::vector<Token>& tokens, bool is_multi);
    void make_number(std::vector<Token>& tokens);
    bool traverse_table(std::vector<Token>& tokens);
    std::string make_identifier();
    void skip_white_spaces();
    std::string _source;
    std::uint32_t _global_cursor;
    std::uint32_t _local_cursor;
    std::uint32_t _line;
};
