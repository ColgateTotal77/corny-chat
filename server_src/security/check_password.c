#include <password.h>

bool check_password(char *password) { //Валідація паролю
    if (password == NULL) { //Перевірка на порожній рядок
        return false;
    }

    int password_length = mx_strlen(password);

    if (password_length < 8 || password_length > 20) { //Перевірка на довжину, яка має бути більше 7 і менше 20
        return false;
    }

    for (int i = 0; i < password_length; i++) {
        if (mx_check_space(password[i]) || password[i] < 33 || password[i] > 126) { //Перевірка пароля на невалідні символи по ASCII, тут дозволені від '!' до '~'
            return false;
        }
    }
    return true;
}

