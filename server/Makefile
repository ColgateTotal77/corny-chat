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

KEY = server.key
CERT = server.crt

SERVER = server
LIBRARY = ../libmx/libmx.a
JSON_C = ../json-c/json-c.a

.PHONY: all clean uninstall reinstall

# Компиляция серверного и клиентского приложений
all: keys $(SERVER) #$(CLIENT)

keys: $(KEY) $(CERT)

$(SERVER): $(OBJFILES)
	$(CC) $(CFLAGS) -pthread $(OBJFILES) -o $(SERVER) $(LIBRARY) $(JSON_C) $(LDFLAGS)

$(KEY):
	openssl genrsa -out $(KEY) 2048
	@echo "Generated private key: $(KEY)"

$(CERT): $(KEY)
	openssl req -new -x509 -key $(KEY) -out $(CERT) -days 365 -subj "/CN=localhost"
	@echo "Generated certificate: $(CERT)"

# Правило для создания объектных файлов и нужных подкаталогов
$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES) -I $(GENERAL_HEADERFILES)

# Удаление всех файлов
uninstall: clean
	rm -f $(SERVER) $(KEY) $(CERT)

# Очистка объектных файлов
clean:
	rm -rf $(OBJ)

# Переустановка
reinstall: uninstall all
