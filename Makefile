CPP = gcc -E -w 	# preprocessor.
vA?= 1				# ast printing level: 1=minimal, 2=middle, 3=verbose.
vQ?= 1				# quads printing level- defaults to print

# my testing make
my-test-compiler: flex-bison frontEndHeaders.o symbol_table.o quads.o pheader_ast.o compiler_test.o pheaders.o 
	gcc -o guycc frontEndHeaders.o compiler_test.o pheaders.o quads.o pheader_ast.o symbol_table.o
	$(CPP) ./back-end/tests_6/my_test.c | ./guycc -p $(vA) $(vQ)



# partial builds
create-parser:
	bison -v -d -o ./front-end/parser/parser.c ./front-end/parser/parser.y -Wconflicts-sr

create-lexer:
	flex -o ./front-end/lexer/lexer.c ./front-end/lexer/lexer.l 

flex-bison: create-lexer create-parser

quads.o: ./front-end/parser/quads.h ./front-end/parser/quads.c
	gcc -c ./front-end/parser/quads.c

compiler_test.o: ./compiler_test.c ./front-end/parser/parser.c ./front-end/lexer/lexer.c 
	gcc -c ./compiler_test.c

pheaders.o: ./front-end/lexer/lheader.h ./front-end/lexer/lheader2.h ./front-end/lexer/lheader2.c 
	gcc -o pheaders.o -c ./front-end/lexer/lheader2.c

frontEndHeaders.o: ./front-end/front_end_header.h ./front-end/front_end_header.c
	gcc -o frontEndHeaders.o -c ./front-end/front_end_header.c

pheader_ast.o: ./front-end/parser/pheader_ast.c ./front-end/parser/pheader_ast.h ./front-end/parser/symbol_table.h
	gcc -c ./front-end/parser/pheader_ast.c

symbol_table.o: ./front-end/parser/symbol_table.h ./front-end/parser/symbol_table.c
	gcc -c ./front-end/parser/symbol_table.c

clear:
	rm  *.o .front-end/lexer/lexer.c ./front-end/parser/parser.c ./front-end/parser/parser.output ./parser