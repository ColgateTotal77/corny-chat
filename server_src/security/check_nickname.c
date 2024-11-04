#include <password.h>

bool check_nickname(char *nickname) {
    if (nickname == NULL) {
        return false;
    }

    int nickname_length = mx_strlen(nickname);
    if (nickname_length < 3 || nickname_length > 20) {
        return false;
    }

    for (int i = 0; i < nickname_length; i++) {
        if (mx_check_space(nickname[i]) || !((nickname[i] >= '0' && nickname[i] <= '9') || 
        (nickname[i] >= 'A' && nickname[i] <= 'Z') || (nickname[i] >= 'a' && nickname[i] <= 'z')
        || nickname[i] == '_')) {

            return false;
        }
    }
    return true;
}

