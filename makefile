all: hello

hello: test_signal_generator.c
	gcc -Wall test_signal_generator.c -o test_signal_generator -lm -lpanel -lmenu -lform -lncurses

