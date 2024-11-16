# Директории заголовков и библиотеки
LIBRARYDIR = ./libmx
JSON_C_DIR = ./json-c

# Папки для серверных и клиентских файлов
SERVER_DIR = ./server
CLIENT_DIR = ./client


SERVER = server/server
CLIENT = client/client
LIBRARY = libmx/libmx.a
JSON_C = json-c/json-c.a

.PHONY: all clean uninstall reinstall $(LIBRARY)

# Компиляция серверного и клиентского приложений
all: $(SERVER) $(CLIENT)

$(SERVER): $(LIBRARY) $(JSON_C)
	cd $(SERVER_DIR) && make

$(CLIENT): $(LIBRARY) $(JSON_C)
	cd $(CLIENT_DIR) && make

# Компиляция библиотек
$(LIBRARY):
	cd $(LIBRARYDIR) && make

$(JSON_C):
	cd $(JSON_C_DIR) && make


# Удаление всех файлов
uninstall: clean
	cd $(LIBRARYDIR) && make uninstall
	cd $(JSON_C_DIR) && make uninstall
	cd $(SERVER_DIR) && make uninstall
	cd $(CLIENT_DIR) && make uninstall

# Очистка объектных файлов
clean:
	cd $(LIBRARYDIR) && make clean
	cd $(JSON_C_DIR) && make clean
	cd $(SERVER_DIR) && make clean
	cd $(CLIENT_DIR) && make clean

# Переустановка
reinstall: uninstall all
