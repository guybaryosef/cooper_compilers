CPP = gcc -E	# preprocessor


test-lexer:	create-lexer lheaders.o lexer-tester.o 
	gcc -o ./my_test/lexer lheaders.o lexer-tester.o -lm
	$(CPP) ltests/*.c | ./my_test/lexer >./my_test/my_results.out 2>./my_test/my_results.err
	diff ./my_test/my_results.out ./ltests/ltest.out


test-lexer2:	create-lexer lheaders.o lexer-tester.o 
	gcc -o ./my_test/lexer lheaders.o lexer-tester.o -lm
	$(CPP) ltests/num.c | ./my_test/lexer >./my_test/my_results.out 2>./my_test/my_results.err

lexer-tester.o: ./my_test/lexer-tester.c
	gcc -c ./my_test/lexer-tester.c

create-lexer: lexer.l
	flex -o lexer.c lexer.l

lheaders.o: lheader.h lheader2.c lheader2.h
	gcc -o lheaders.o -c lheader2.c

clear:
	rm  ./my_test/my_results* ./lexer.c ./lexer ./my_test/lexer *.o