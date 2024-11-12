#pragma once

#include "../libmx/inc/libmx.h"
#include <openssl/sha.h>
#include <time.h>

char *generate_password(void);
unsigned char *hash_password(char *password, char *login);
unsigned char *create_admin(void);
bool check_login(char *login);
bool check_password(char *password);

