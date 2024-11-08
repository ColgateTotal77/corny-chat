-- Создание таблиц
CREATE TABLE IF NOT EXISTS roles
(
    id     INTEGER PRIMARY KEY AUTOINCREMENT,
    'name' TEXT NOT NULL UNIQUE
);

INSERT OR IGNORE INTO roles (name)
VALUES ('admin');
INSERT OR IGNORE INTO roles (name)
VALUES ('user');

CREATE TABLE IF NOT EXISTS users
(
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    createdAt DATETIME         DEFAULT CURRENT_TIMESTAMP,
    login     TEXT    NOT NULL UNIQUE,
    password  BLOB,
    nickname  TEXT,
    roleId    INTEGER          DEFAULT 2,
    active    BOOLEAN NOT NULL DEFAULT TRUE,
    FOREIGN KEY (roleId) REFERENCES roles (id) ON DELETE CASCADE
    );

INSERT OR IGNORE INTO users (login, nickname, roleId)
VALUES ('FirstAdmin', 'FirstAdmin', 1);

CREATE TABLE IF NOT EXISTS 'groups'
(
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    createdAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    name      TEXT    NOT NULL,
    ownerId   INTEGER NOT NULL,
    FOREIGN KEY (ownerId) REFERENCES users (id) ON DELETE CASCADE
    );

CREATE TABLE IF NOT EXISTS 'group_users'
(
    groupId INTEGER NOT NULL,
    userId  INTEGER NOT NULL,
    FOREIGN KEY (groupId) REFERENCES groups (id),
    FOREIGN KEY (userId) REFERENCES users (id)
    );

CREATE TABLE IF NOT EXISTS messages
(
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    createdAt     DATETIME DEFAULT CURRENT_TIMESTAMP,
    ownerId       INTEGER NOT NULL,
    targetUserId  INTEGER,
    targetGroupId INTEGER,
    message       TEXT,
    readed        BOOLEAN DEFAULT FALSE,
    sKey          BLOB,
    FOREIGN KEY (ownerId) REFERENCES users (id) ON DELETE CASCADE,
    FOREIGN KEY (targetUserId) REFERENCES users (id) ON DELETE CASCADE,
    FOREIGN KEY (targetGroupId) REFERENCES groups (id) ON DELETE CASCADE,
    CHECK (targetUserId IS NOT NULL OR targetGroupId IS NOT NULL)

    );