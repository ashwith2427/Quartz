cmake_minimum_required(VERSION 3.20)
project(SubC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src/lexer)
add_subdirectory(src/parser)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        parser lexer
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

configure_file(src/sample.cext ${CMAKE_BINARY_DIR}/sample.cext COPYONLY)
