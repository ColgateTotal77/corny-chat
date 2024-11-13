# Компилятор и флаги
CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -D_POSIX_C_SOURCE
LDFLAGS = -lssl -lcrypto
# Директории заголовков и библиотеки
HEADERFILES = ./inc
LIBRARYDIR = ./libmx
KONST_LIBRARYDIR = ./libmx_konst
JSON_C_DIR = ./json-c

# Папки для серверных и клиентских файлов
SERVER_OBJ = ./server_obj
SERVER_SRC = ./server_src
CLIENT_OBJ = ./client_obj
CLIENT_SRC = ./client_src

# Список исходных файлов (ищет все .c файлы, включая вложенные)
SERVER_SRCFILES := $(shell find $(SERVER_SRC) -type f -name '*.c')
SERVER_OBJFILES := $(patsubst $(SERVER_SRC)/%.c, $(SERVER_OBJ)/%.o, $(SERVER_SRCFILES))

CLIENT_SRCFILES := $(shell find $(CLIENT_SRC) -type f -name '*.c')
CLIENT_OBJFILES := $(patsubst $(CLIENT_SRC)/%.c, $(CLIENT_OBJ)/%.o, $(CLIENT_SRCFILES))

SERVER = server
CLIENT = client
LIBRARY = libmx/libmx.a
KONST_LIBRARY = libmx_konst/libmx.a
JSON_C = json-c/json-c.a

.PHONY: all clean uninstall reinstall $(LIBRARY)

# Компиляция серверного и клиентского приложений
all: $(SERVER) $(CLIENT)


$(SERVER): $(LIBRARY) $(KONST_LIBRARY) $(JSON_C) $(SERVER_OBJFILES)
	$(CC) $(CFLAGS) -pthread $(SERVER_OBJFILES) -o $(SERVER) $(LIBRARY) $(KONST_LIBRARY) $(JSON_C) $(LDFLAGS) -lsqlite3

$(CLIENT): $(LIBRARY) $(JSON_C) $(CLIENT_OBJFILES)
	$(CC) $(CFLAGS) -pthread $(CLIENT_OBJFILES) -o $(CLIENT) $(LIBRARY) $(JSON_C) $(LDFLAGS)


# Компиляция библиотеки
$(LIBRARY):
	cd $(LIBRARYDIR) && make

$(KONST_LIBRARY):
	cd $(KONST_LIBRARYDIR) && make

$(JSON_C):
	cd $(JSON_C_DIR) && make

# Правило для создания объектных файлов и нужных подкаталогов
$(SERVER_OBJ)/%.o: $(SERVER_SRC)/%.c
	@mkdir -p $(dir $@)   # Создает директорию для объекта, если ее нет
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES)

# Аналогичное правило для клиентских файлов
$(CLIENT_OBJ)/%.o: $(CLIENT_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES)

# Удаление всех файлов
uninstall: clean
	cd $(LIBRARYDIR) && make uninstall
	cd $(KONST_LIBRARYDIR) && make uninstall
	cd $(JSON_C_DIR) && make uninstall
	rm -f $(SERVER) $(CLIENT)

# Очистка объектных файлов
clean:
	cd $(LIBRARYDIR) && make clean
	cd $(KONST_LIBRARYDIR) && make clean
	cd $(JSON_C_DIR) && make clean
	rm -rf $(SERVER_OBJ) $(CLIENT_OBJ)

# Переустановка
reinstall: uninstall all
