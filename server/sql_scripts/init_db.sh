#!/bin/bash

# launch from the root directory ./sql_scripts/init_db.sh
# Задаем путь к папке и файлу базы данных
DB_DIR="db"
DB_FILE="$DB_DIR/uchat_db.db"
SQL_SCRIPT="sql_scripts/init_db.sql"

# Проверка, существует ли папка 'bd', если нет - создаем её
if [ ! -d "$DB_DIR" ]; then
    echo "Создаю папку $DB_DIR..."
    mkdir "$DB_DIR"
fi

# Проверка, существует ли база данных, если нет - создаем пустую
if [ ! -f "$DB_FILE" ]; then
    echo "Создаю базу данных $DB_FILE..."
    sqlite3 "$DB_FILE" "VACUUM;"  # Команда VACUUM создаст пустую базу данных
else
    echo "База данных уже существует."
fi

# Выполнение SQL-скрипта для создания таблицы
if [ -f "$SQL_SCRIPT" ]; then
    echo "Исполняю скрипт $SQL_SCRIPT для создания таблиц..."
    sqlite3 "$DB_FILE" < "$SQL_SCRIPT"
    echo "Таблицы успешно созданы или уже существуют."
else
    echo "Скрипт $SQL_SCRIPT не найден!"
fi