CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

OBJDIR = ./obj
SRCDIR = ./src
HEADERFILES = ../inc

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCFILES))

JSON_C = json-c.a

all: $(JSON_C)

$(JSON_C): $(OBJFILES)
	ar rcs $@ $^

$(OBJFILES): | $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERFILES)
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES)

$(OBJDIR):
	mkdir -p $@

uninstall: clean
	rm -f $(JSON_C)

clean:
	rm -r -f $(OBJDIR)

reinstall: uninstall all