CPP = gcc -E -w	# preprocessor


official-test-lexer: create-lexer lheaders.o lexer-tester.o 
	gcc -o ./my_test/lexer lheaders.o lexer-tester.o -lm
	$(CPP) ltests/*.c | ./my_test/lexer >./my_test/my_results.out 2>./my_test/my_results.err
	diff ./my_test/my_results.out ./ltests/ltest.out

my-test-lexer: create-lexer lexer-tester.o lheaders.o 
	gcc -o ./my_test/lexer lheaders.o lexer-tester.o -lm
	$(CPP) my_test/test1.c | ./my_test/lexer >./my_test/my_results.out 2>./my_test/my_results.err

lexer-tester.o: ./my_test/lexer-tester.c ./lexer.c
	gcc -c ./my_test/lexer-tester.c

create-lexer: lexer.l
	flex -o lexer.c lexer.l

lheaders.o: lheader.h lheader2.h lheader2.c ../front_end_header.h
	gcc -o lheaders.o -c lheader2.c

clear:
	rm  ./my_test/my_results* ./lexer.c ./lexer ./my_test/lexer *.o