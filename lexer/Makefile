CPP = gcc -E	# preprocessor

make-lexer:
	flex -o lexer.c myLexer.l 

test-lexer:
	flex -o lexer.c myLexer.l 
	gcc ./my_test/lexer-tester.c -o ./my_test/lexer -lm
	$(CPP) ltests/*.c | ./my_test/lexer >./my_test/my_results.out 2>./my_test/my_results.err
	diff ./my_test/my_results.out ./ltests/ltest.out

clear:
	rm  ./my_test/my_results* ./lexer.c ./lexer ./my_test/lexer