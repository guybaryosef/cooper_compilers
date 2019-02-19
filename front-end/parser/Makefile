CPP = gcc -E -w	# preprocessor


create-parser:
	bison -v -d -o ./parser.c parser.y -Wconflicts-sr

create-lexer:
	flex -o ../lexer/lexer.c ../lexer/lexer.l 

flex-bison: create-lexer create-parser

my-test-parser: flex-bison frontEndHeaders.o pheader_ast.o parser_tester.o pheaders.o 
	gcc -o parser frontEndHeaders.o parser_tester.o pheaders.o  pheader_ast.o
	$(CPP) tests/my_test.c | ./parser 
# >./tests/my_test1.out 2>./tests/my_test1.err

official-test-parser: flex-bison frontEndHeaders.o parser_tester.o pheaders.o pheader_ast.o
	gcc -o parser frontEndHeaders.o parser_tester.o pheaders.o  pheader_ast.o
	$(CPP) tests/exprtests.c | ./parser >./tests/my_results.out 2>./tests/my_results.err
	diff ./tests/my_results.out ./tests/exprtests.out

parser_tester.o: parser_tester.c parser.c ../lexer/lexer.c 
	gcc -c ./parser_tester.c

pheaders.o: ../lexer/lheader.h ../lexer/lheader2.h ../lexer/lheader2.c 
	gcc -o pheaders.o -c ../lexer/lheader2.c

frontEndHeaders.o: ../front_end_header.h ../front_end_header.c
	gcc -o frontEndHeaders.o -c ../front_end_header.c

pheader_ast.o: pheader_ast.c pheader_ast.h
	gcc -c pheader_ast.c

clear:
	rm  ../lexer/lexer.c ./parser.c ./parser.output ./parser ./tests/my_results* ./tests/my_test1* *.o