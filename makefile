all: hello

hello: test_signal_generator.c
	gcc -lncurses -Wall test_signal_generator.c -o test_signal_generator -lm

