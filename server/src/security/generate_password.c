#include <password.h>

char *generate_password(void) { //Функція колись використовувалася замість логіну у шифруванні, можна буде перевикористовувати для створення рандомного поролю
    srand(time(NULL));
    int passwordLen = rand() % 8 + 8; //от 8 до 16
    char *password = mx_strnew(passwordLen);

    for (int i = 0; i < passwordLen ; i++) {
        switch (rand() % 3)
        {
        case 0:
            password[i] = '0' + rand() % 10;
            break;
        case 1:
            password[i] = 'A' + rand() % 26;
            break;
        case 2:
            password[i] = 'a' + rand() % 26;
            break;
        default:
            break;
        }
    }
    password[passwordLen] = '\0';
    return password;
}

