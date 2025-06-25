#include "lexer/lexer.hpp"
#include <iostream>

using namespace lexer;
int main()
{
    Lexer l("build/sample.cext");
    std::vector<Token> tokens = l.scan();
    for (auto const& token : tokens) {
        std::cout << token << '\n';
    }
    return 0;
}
