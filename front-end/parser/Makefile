CPP = gcc -E -w 	# preprocessor


# my testing make
my-test-parser: flex-bison frontEndHeaders.o symbol_table.o pheader_ast.o parser_tester.o pheaders.o 
	gcc -o parser frontEndHeaders.o parser_tester.o pheaders.o  pheader_ast.o symbol_table.o
	$(CPP) tests_3/my_test.c | ./parser

# tests for assignment 2
official-test-parser2: flex-bison frontEndHeaders.o parser_tester.o pheaders.o pheader_ast.o
	gcc -o parser frontEndHeaders.o parser_tester.o pheaders.o  pheader_ast.o
	$(CPP) tests_2/exprtests.c | ./parser >./tests_2/my_results.out 2>./tests_2/my_results.err
	diff ./tests_2/my_results.out ./tests_2/exprtests.out

# tests for assignment 3
official-test-parser3: flex-bison frontEndHeaders.o parser_tester.o pheaders.o pheader_ast.o symbol_table.o
	gcc -o parser frontEndHeaders.o parser_tester.o pheaders.o  pheader_ast.o symbol_table.o
	$(CPP) tests_3/base.c | ./parser >./tests_3/MYRESULTS_base.out 2>./tests_2/MYRESULTS_base.out
	$(CPP) tests_3/fn1.c | ./parser >./tests_3/MYRESULTS_fn1.out 2>./tests_2/MYRESULTS_fn1.out
	$(CPP) tests_3/scope1.c | ./parser >./tests_3/MYRESULTS_scope1.out 2>./tests_2/MYRESULTS_scope1.out
	$(CPP) tests_3/struct1.c | ./parser >./tests_3/MYRESULTS_struct1.out 2>./tests_2/MYRESULTS_struct1.out
	$(CPP) tests_3/struct2.c | ./parser >./tests_3/MYRESULTS_struct2.out 2>./tests_2/MYRESULTS_struct2.out
	$(CPP) tests_3/td1.c | ./parser >./tests_3/MYRESULTS_td1.out 2>./tests_2/MYRESULTS_td1.out

# partial builds
create-parser:
	bison -v -d -o ./parser.c parser.y -Wconflicts-sr

create-lexer:
	flex -o ../lexer/lexer.c ../lexer/lexer.l 

flex-bison: create-lexer create-parser

parser_tester.o: parser_tester.c parser.c ../lexer/lexer.c 
	gcc -c ./parser_tester.c

pheaders.o: ../lexer/lheader.h ../lexer/lheader2.h ../lexer/lheader2.c 
	gcc -o pheaders.o -c ../lexer/lheader2.c

frontEndHeaders.o: ../front_end_header.h ../front_end_header.c
	gcc -o frontEndHeaders.o -c ../front_end_header.c

pheader_ast.o: pheader_ast.c pheader_ast.h symbol_table.h
	gcc -c pheader_ast.c

symbol_table.o: symbol_table.c symbol_table.h

clear:
	rm  *.o ../lexer/lexer.c ./parser.c ./parser.output ./parser ./tests_2/my_results* ./tests_2/my_test1* ./tests_3/MYRESULTS_*