cmake_minimum_required(VERSION 3.13)
project(compilers)

set(CMAKE_CXX_STANDARD 14)

include_directories(front-end/lexer)
include_directories(front-end/lexer/ltests)
include_directories(front-end/parser)

add_executable(compilers
        front-end/lexer/ltests/chars.c
        front-end/lexer/ltests/kw.c
        front-end/lexer/ltests/ltest.err
        front-end/lexer/ltests/ltest.out
        front-end/lexer/ltests/num.c
        front-end/lexer/ltests/op.c
        front-end/lexer/ltests/README
        front-end/lexer/ltests/tokens-manual.h
        front-end/lexer/my_test/lexer-tester.c
        front-end/lexer/assign1.pdf
        front-end/lexer/lexer.c
        front-end/lexer/lexer.l
        front-end/lexer/lheader.h
        front-end/lexer/lheader2.c
        front-end/lexer/lheader2.h
        front-end/lexer/Makefile
        front-end/parser/tests/exprtests.c
        front-end/parser/tests/exprtests.out
        front-end/parser/tests/my_test.c
        front-end/parser/assign2.pdf
        front-end/parser/lheaders.o
        front-end/parser/Makefile
        front-end/parser/parser
        front-end/parser/parser.c
        front-end/parser/parser.output
        front-end/parser/parser.y
        front-end/parser/parser_tester.c
        front-end/parser/parser_tester.o
        front-end/parser/pheader_ast.c
        front-end/parser/pheader_ast.h
        front-end/parser/pheader_ast.o
        front-end/parser/pheader_lex_comp.h
        lectures/lecture-u01.pdf
        lectures/lecture-u02.pdf
        README.md)
