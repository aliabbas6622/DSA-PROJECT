cmake_minimum_required(VERSION 3.10)
project(GraphX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(graphx
    src/main.cpp
    src/Expression.cpp
    src/ExpressionParser.cpp
    src/EvaluationCache.cpp
    src/EquationHistory.cpp
    src/GraphSampler.cpp
    ui/ConsoleApp.cpp
    ui/SvgRenderer.cpp
)

target_include_directories(graphx PRIVATE include ui)

if(MSVC)
    target_compile_options(graphx PRIVATE /W4)
else()
    target_compile_options(graphx PRIVATE -Wall -Wextra -pedantic)
endif()
