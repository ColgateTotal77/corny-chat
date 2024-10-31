# Проверка и установка SQLite, если не установлен
if ! command -v sqlite3 &> /dev/null; then
    echo "SQLite не установлен. Устанавливаю..."
#    sudo apt update
    sudo apt install -y sqlite3 libsqlite3-dev
else
    echo "SQLite уже установлен."
fi

# Здесь можно добавлять другие проверки и установки