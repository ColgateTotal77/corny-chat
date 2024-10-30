CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -D_POSIX_C_SOURCE

HEADERFILES = ./inc
LIBRARYDIR = ./libmx

SERVER_OBJ = ./server_obj
SERVER_SRC = ./server_src
SERVER_SRCFILES := $(wildcard $(SERVER_SRC)/*.c)
SERVER_OBJFILES := $(patsubst $(SERVER_SRC)/%.c, $(SERVER_OBJ)/%.o, $(SERVER_SRCFILES))

CLIENT_OBJ = ./client_obj
CLIENT_SRC = ./client_src
CLIENT_SRCFILES := $(wildcard $(CLIENT_SRC)/*.c)
CLIENT_OBJFILES := $(patsubst $(CLIENT_SRC)/%.c, $(CLIENT_OBJ)/%.o, $(CLIENT_SRCFILES))

SERVER = server
CLIENT = client
LIBRARY = libmx/libmx.a

.PHONY: $(LIBRARY)

all: $(SERVER) $(CLIENT)

$(SERVER): $(LIBRARY) $(SERVER_OBJFILES)
	${CC} ${CFLAG} -pthread ${SERVER_OBJFILES} -o ${SERVER} $(LIBRARY)

$(CLIENT): $(LIBRARY) $(CLIENT_OBJFILES)
	${CC} ${CFLAG} -pthread ${CLIENT_OBJFILES} -o ${CLIENT} $(LIBRARY)

$(LIBRARY):
	cd $(LIBRARYDIR) && make

$(SERVER_OBJFILES): | $(SERVER_OBJ)

$(SERVER_OBJ)/%.o: $(SERVER_SRC)/%.c $(HEADERFILES)
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES)

$(SERVER_OBJ):
	mkdir -p $@

$(CLIENT_OBJFILES): | $(CLIENT_OBJ)

$(CLIENT_OBJ)/%.o: $(CLIENT_SRC)/%.c $(HEADERFILES)
	$(CC) $(CFLAGS) -o $@ -c $< -I $(HEADERFILES)

$(CLIENT_OBJ):
	mkdir -p $@

uninstall: clean
	cd $(LIBRARYDIR) && make uninstall
	rm -f $(SERVER)
	rm -f $(CLIENT)

clean:
	cd $(LIBRARYDIR) && make clean
	rm -r -f $(CLIENT_OBJ)
	rm -r -f $(SERVER_OBJ)

reinstall: uninstall all
