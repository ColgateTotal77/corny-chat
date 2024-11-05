#pragma once

#include "../libmx/inc/libmx.h"
#include <openssl/sha.h>
#include <time.h>

char *generate_salt(void);
unsigned char *hash_password(char *password, char *salt);
bool check_nickname(char *nickname);
bool check_password(char *password);
void data_validation(char *nickname, char *password);

