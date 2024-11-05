#pragma once

#include "../libmx/inc/libmx.h"
#include <openssl/sha.h>
#include <time.h>

char *generate_salt(void);
unsigned char *hash_password(char *password, char *login);
bool check_login(char *login);
bool check_password(char *password);


