# Проверка и установка SQLite, если не установлен
if ! command -v sqlite3 &> /dev/null; then
    echo "SQLite не установлен. Устанавливаю..."
    # sudo apt update
    sudo apt install -y sqlite3 libsqlite3-dev
else
    echo "SQLite уже установлен."
fi

# Проверка и установка libssl-dev, если не установлен
if ! dpkg -s libssl-dev &> /dev/null; then
    echo "libssl-dev не установлен. Устанавливаю..."
    # sudo apt update
    sudo apt install -y libssl-dev
else
    echo "libssl-dev уже установлен."
fi

# Проверка и установка libgtk-4-dev, если не установлен
if ! dpkg -s libgtk-4-dev &> /dev/null; then 
    echo "libgtk-4-dev не установлен. Устанавливаю..."
    # sudo apt update
    sudo apt install -y libgtk-4-dev #Потрібно додати версію
else
    echo "libgtk-4-dev уже установлен."
fi