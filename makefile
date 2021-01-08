SHELL	= /bin/sh
CC	= gcc
LINKER	= gcc

TARGET	= test_signal_generator
SRCDIR	= src
INCLUDE	= include
OBJDIR	= build

OBJS	= $(addprefix $(OBJDIR)/, input_validation.o form_handler.o test_signal_generator.o)

CFLAGS	= -g -O0 -Wall -Wextra -pedantic
LDLIBS	= -lm -lpanel -lmenu -lform -lncurses

# Search paths
vpath %.o $(OBJDIR)
vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR) $(INCLUDE)

all : $(TARGET)
$(TARGET) : $(OBJS)
	$(LINKER) -o $@ $^ $(LDLIBS) $(CFLAGS)

$(OBJS): | $(OBJDIR)
$(OBJDIR)/%.o : %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJDIR) :
	mkdir $(OBJDIR)

.PHONY: clean
clean :
	-rm -f $(TARGET) $(OBJDIR)/*.o
