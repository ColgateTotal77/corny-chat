# Название библиотеки
NAME = libmx.a

# Автоматически находить все .c файлы в папке src
SRCS = $(wildcard src/*.c)

# Соответствующие объектные файлы
OBJS = $(SRCS:src/%.c=obj/%.o)

# Флаги компиляции
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

# Путь к заголовочным файлам
INC = -Iinc

# Правило по умолчанию: сборка библиотеки
all: $(NAME)

# Как создать объектные файлы из исходных
obj/%.o: src/%.c
	@mkdir -p obj
	clang $(CFLAGS) $(INC) -c $< -o $@

# Как создать библиотеку из объектных файлов
$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

# Очистка объектных файлов и библиотеки
clean:
	rm -rf obj

# Полная очистка библиотеки
uninstall: clean
	rm -f $(NAME)

# Пересобрать библиотеку заново
reinstall: uninstall all

