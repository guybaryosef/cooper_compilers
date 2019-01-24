CPP = gcc -E	# preprocessor

make-flex:
	flex -o myLexer.c myLexer.l 
	gcc myLexer.c -o lexer -lm

test-chars:
	$(CPP) ./official_test/chars.c | ./lexer >./my_test/my_test_chars.out 2>./my_test/my_test_chars.err

test-kw:
	$(CPP) ./official_test/kw.c | ./lexer >./my_test/my_test_kw.out 2>./my_test/my_test_kw.err

test-num:
	$(CPP) ./official_test/num.c | ./lexer >./my_test/my_test_num.out 2>./my_test/my_test_num.err

test-op:
	$(CPP) ./official_test/op.c | ./lexer >./my_test/my_test_op.out 2>./my_test/my_test_op.err

my-test-preprocess:
	$(CPP) ./my_test/t1.c > ./my_test/t1.i

my-test:
	$(CPP) ./my_test/t1.c | ./lexer >./my_test/t1.out 2>./my_test/t1.err

clear:
	rm ./my_test/t1.err ./my_test/t1.i ./my_test/t1.out ./myLexer.c ./my_test/my_test_*