#include "lexer/lexer.hpp"
#include <iostream>

bool match(std::string_view matcher, std::string_view source)
{
    return source.substr(0).starts_with(matcher);
}

int main()
{
    Lexer l("build/sample.cext");
    std::vector<Token> tokens = l.scan();
    for (const auto& token : tokens) {
        std::cout << TokenKindToString(token.kind()) << '\n';
    }
    return 0;
}
