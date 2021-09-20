all:second

second:second.c
	gcc -g -std=c11 -Wall -Werror -fsanitize=address second.c -o second -lm
clean:
	rm -f second

