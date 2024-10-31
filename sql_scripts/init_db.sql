-- Создание таблицы users
CREATE TABLE IF NOT EXISTS users (
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                                     login TEXT NOT NULL UNIQUE,
                                     password TEXT,
                                     nickname TEXT
    );

INSERT OR IGNORE INTO users (login, nickname) VALUES ('FirstAdmin', 'FirstAdmin');

CREATE TABLE IF NOT EXISTS messages (
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                                     user_id INTEGER NOT NULL,
                                     message TEXT,
                                     FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE
    );