all: treePipe left right

clean:
	rm -f treePipe left right

treePipe: treePipe.c
	gcc -o treePipe treePipe.c -Wall

left: pl.c
	gcc -o left pl.c -Wall

right: pr.c
	gcc -o right pr.c -Wall


