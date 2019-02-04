CPP = gcc -E	# preprocessor


test-lexer:	create-lexer lheader.o lexer-tester.o 
	gcc -o ./my_test/lexer lheader.o lexer-tester.o -lm
	$(CPP) ltests/*.c | ./my_test/lexer >./my_test/my_results.out 2>./my_test/my_results.err
	diff ./my_test/my_results.out ./ltests/ltest.out

lexer-tester.o: ./my_test/lexer-tester.c
	gcc -c ./my_test/lexer-tester.c

create-lexer: lexer.l
	flex -o lexer.c lexer.l

lheader.o: lheader.c lheader.h
	gcc -c lheader.c

clear:
	rm  ./my_test/my_results* ./lexer.c ./lexer ./my_test/lexer *.o