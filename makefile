CC=gcc
TARGET=test_signal_generator
OBJDIR=build
OBJS=$(addprefix $(OBJDIR)/, ASCII_parser.o test_signal_generator.o)
CFLAGS=-Wall -Wextra -pedantic -g -O0
LDLIBS=-lm -lpanel -lmenu -lform -lncurses

# Search path for .o files
vpath %.o $(OBJDIR)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(OBJS): | $(OBJDIR)
$(OBJDIR)/%.o : %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR) :
	mkdir $(OBJDIR)

.PHONY: clean
clean :
	-rm -f $(TARGET) $(OBJDIR)/*.o
