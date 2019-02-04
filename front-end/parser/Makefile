CPP = gcc -E	# preprocessor


create-parser:
	bison -v -o ./parser.c parser.y

create-lexer:
	flex -o ../lexer/lexer.c ../lexer/lexer.l 

flex-bison: create-lexer create-parser

test-parser: flex-bison parser_tester.o lheader.o pheader_lex_comp.o pheader_ast.o
	gcc -o parser parser_tester.o lheader.o pheader_lex_comp.o pheader_ast.o
	$(CPP) tests/exprtests.c | ./parser >./tests/my_results.out 2>./tests/my_results.err

parser_tester.o: ./parser_tester.c
	gcc -c ./parser_tester.c

lheader.o: ../lexer/lheader.h ../lexer/lheader.c
	gcc -c ../lexer/lheader.c

pheader_lex_comp.o: ./pheader_lex_comp.h ./pheader_lex_comp.c
	gcc -c pheader_lex_comp.c

pheader_ast.o: pheader_ast.c pheader_ast.h
	gcc -c pheader_ast.c

clear:
	rm  ../lexer/lexer.c ./parser.c ./parser.output ./parser ./tests/my_results* *.o