# Удаление SQLite и связанных пакетов
echo "Удаляю SQLite и связанные пакеты..."
sudo apt remove --purge -y sqlite3 libsqlite3-dev

# Дополнительные пакеты можно добавить здесь
# echo "Удаляю другие пакеты..."
# sudo apt remove --purge -y <other_packages>

# Очистка неиспользуемых пакетов
sudo apt autoremove -y