add_library(parser SHARED parser.cc)

target_include_directories(parser
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(parser
    PUBLIC
        lexer
)
