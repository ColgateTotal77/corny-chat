# Компилятор и флаги
CC = clang

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -D_POSIX_C_SOURCE
LDFLAGS = -lssl -lcrypto
GTK_CFLAGS = `pkg-config --cflags gtk4`
GTK_LIBS = `pkg-config --libs gtk4`

# Директории заголовков и библиотеки
HEADERFILES = ./inc
GENERAL_HEADERFILES = ../inc

# Папки для серверных и клиентских файлов
OBJ = ./obj
SRC = ./src

# Список исходных файлов (ищет все .c файлы, включая вложенные)
SRCFILES := $(shell find $(SRC) -type f -name '*.c')
OBJFILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCFILES))

CLIENT = client
LIBRARY = ../libmx/libmx.a
JSON_C = ../json-c/json-c.a

.PHONY: all clean uninstall reinstall

# Компиляция серверного и клиентского приложений
all: $(SERVER) $(CLIENT)


$(CLIENT): $(LIBRARY) $(JSON_C) $(OBJFILES)
	$(CC) $(CFLAGS) -pthread $(OBJFILES) -o $(CLIENT) $(LIBRARY) $(JSON_C) $(LDFLAGS) $(GTK_LIBS)


# Правило для создания объектных файлов и нужных подкаталогов
$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $@ -c $< -I $(HEADERFILES) -I $(GENERAL_HEADERFILES)

# Удаление всех файлов
uninstall: clean
	rm -f $(CLIENT)

# Очистка объектных файлов
clean:
	rm -rf $(OBJ)

# Переустановка
reinstall: uninstall all
