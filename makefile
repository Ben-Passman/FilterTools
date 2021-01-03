all: hello

hello: test_signal_generator.c
	gcc -Wall ASCII_parser.c test_signal_generator.c -o test_signal_generator -lm -lpanel -lmenu -lform -lncurses

