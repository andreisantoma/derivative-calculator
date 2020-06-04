all: clean bison flex executable

clean:
	rm -f derivates

bison:
	bison -d derivates.y

flex:
	flex derivates.l

executable:
	gcc derivates.tab.c lex.yy.c source_files/symexpr.c -lfl -o derivates

