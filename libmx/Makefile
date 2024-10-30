CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

OBJDIR = ./obj
SRCDIR = ./src
HEADERFILES = ./inc

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCFILES))

MINILIBMX = libmx.a

all: $(MINILIBMX)

$(MINILIBMX): $(OBJFILES)
	ar rcs $@ $^

$(OBJFILES): | $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERFILES)
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES)

$(OBJDIR):
	mkdir -p $@

uninstall: clean
	rm -f $(MINILIBMX)

clean:
	rm -r -f $(OBJDIR)

reinstall: uninstall all
