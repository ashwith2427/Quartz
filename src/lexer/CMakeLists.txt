add_library(lexer SHARED lexer.cc)

target_include_directories(lexer
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
