CPP = gcc -E -w	# preprocessor
ast?= 1			# ast printing level: 1=none, 2=minimal, 3=verbose.
quad?= 1		# quads printing level- 1=none, 2=berbose. 
input?= ./tests/my_test.c	# input file
output?= stdout				# if not specified, prints to stdout and does not execute


# 
guycc: flex-bison frontEndHeaders.o symbol_table.o quads.o pheader_ast.o compiler_test.o pheaders.o back-end.o backEndHeaders.o
ifeq ($(output),stdout)
	gcc -o guycc frontEndHeaders.o compiler_test.o pheaders.o quads.o pheader_ast.o symbol_table.o back-end.o backEndHeaders.o
	$(CPP) $(input) | ./guycc -p $(ast) $(quad) -n $(output)
else
	gcc -o guycc frontEndHeaders.o compiler_test.o pheaders.o quads.o pheader_ast.o symbol_table.o back-end.o backEndHeaders.o
	$(CPP) $(input) | ./guycc -p $(ast) $(quad) -n $(output)
	cc -m32 $(output)
	./a.out
endif


actual-assembly:
	gcc -S -m32 -O0 ./tests/my_test.c -fno-pic -o actual_test.s
	cat actual_test.s


# partial builds
create-parser:
	bison -v -d -o ./front-end/parser/parser.c ./front-end/parser/parser.y -Wconflicts-sr

create-lexer:
	flex -o ./front-end/lexer/lexer.c ./front-end/lexer/lexer.l 

flex-bison: create-lexer create-parser

back-end.o: ./back-end/assemb_gen.h ./back-end/assemb_gen.c
	gcc -o back-end.o -c ./back-end/assemb_gen.c

quads.o: ./front-end/parser/quads.h ./front-end/parser/quads.c
	gcc -c ./front-end/parser/quads.c

compiler_test.o: ./compiler_test.c ./front-end/parser/parser.c ./front-end/lexer/lexer.c 
	gcc -c ./compiler_test.c

pheaders.o: ./front-end/lexer/lheader.h ./front-end/lexer/lheader2.h ./front-end/lexer/lheader2.c 
	gcc -o pheaders.o -c ./front-end/lexer/lheader2.c

frontEndHeaders.o: ./front-end/front_end_header.h ./front-end/front_end_header.c
	gcc -o frontEndHeaders.o -c ./front-end/front_end_header.c

backEndHeaders.o: ./back-end/back_end_header.h ./back-end/back_end_header.c
	gcc -o backEndHeaders.o -c ./back-end/back_end_header.c

pheader_ast.o: ./front-end/parser/pheader_ast.c ./front-end/parser/pheader_ast.h ./front-end/parser/symbol_table.h
	gcc -c ./front-end/parser/pheader_ast.c

symbol_table.o: ./front-end/parser/symbol_table.h ./front-end/parser/symbol_table.c
	gcc -c ./front-end/parser/symbol_table.c

clear:
	rm my_test a.out *.s *.o ./front-end/lexer/lexer.c .front-end/lexer/lexer.c ./front-end/parser/parser.c ./front-end/parser/parser.output ./guycc