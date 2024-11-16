# Компилятор и флаги
CC = clang

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -D_POSIX_C_SOURCE
LDFLAGS = -lssl -lcrypto -lsqlite3

# Директории заголовков и библиотеки
HEADERFILES = ./inc
GENERAL_HEADERFILES = ../inc

# Папки для серверных и клиентских файлов
OBJ = ./obj
SRC = ./src

# Список исходных файлов (ищет все .c файлы, включая вложенные)
SRCFILES := $(shell find $(SRC) -type f -name '*.c')
OBJFILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCFILES))


SERVER = server
LIBRARY = ../libmx/libmx.a
JSON_C = ../json-c/json-c.a

.PHONY: all clean uninstall reinstall

# Компиляция серверного и клиентского приложений
all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJFILES)
	$(CC) $(CFLAGS) -pthread $(OBJFILES) -o $(SERVER) $(LIBRARY) $(JSON_C) $(LDFLAGS)


# Правило для создания объектных файлов и нужных подкаталогов
$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES) -I $(GENERAL_HEADERFILES)

# Удаление всех файлов
uninstall: clean
	rm -f $(SERVER)

# Очистка объектных файлов
clean:
	rm -rf $(OBJ)

# Переустановка
reinstall: uninstall all
