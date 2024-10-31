#!/bin/bash

# launch from the root directory ./sql_scripts/clean_db.sh
# Задаем путь к папке и файлу базы данных
DB_DIR="db"
DB_FILE="$DB_DIR/uchat_db.db"

# Проверка, существует ли база данных, и её удаление
if [ -f "$DB_FILE" ]; then
    echo "Удаляю базу данных $DB_FILE..."
    rm "$DB_FILE"
else
    echo "База данных не найдена."
fi

# Проверка, существует ли папка bd, и её удаление
if [ -d "$DB_DIR" ]; then
    echo "Удаляю папку $DB_DIR..."
    rmdir "$DB_DIR"
else
    echo "Папка $DB_DIR не найдена."
fi