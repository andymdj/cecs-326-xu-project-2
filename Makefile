all: philosophers

philosophers: philosophers.c
	gcc -pthread philosophers.c -o philosophers

clean:
	rm philosophers