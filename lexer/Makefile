CPP = gcc -E	# preprocessor

make-flex:
	flex -o myLexer.c myLexer.l 
	gcc myLexer.c -o lexer

my-test-preprocess:
	$(CPP) ./my_test/t1.c > ./my_test/t1.i

my-test:
	$(CPP) ./my_test/t1.c | ./lexer >./my_test/t1.out 2>./my_test/t1.err

clear:
	rm ./my_test/t1.err ./my_test/t1.i ./my_test/t1.out ./myLexer.c ./lexer