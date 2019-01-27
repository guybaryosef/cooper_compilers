CPP = gcc -E	# preprocessor

make-flex:
	flex -o myLexer.c myLexer.l 
	gcc myLexer.c -o lexer -lm

test-lexer:
	$(CPP) ./ltests/*.c | ./lexer >./my_test/my_results.out 2>./my_test/my_results.err
	diff ./my_test/my_results.out ./ltests/ltest.out

clear:
	rm  ./my_test/my_results* ./myLexer.c ./lexer