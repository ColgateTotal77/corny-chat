#pragma once

#include <gtk/gtk.h>
#include "client.h"
#include <commands.h>
#include <time.h>


void GTK_start(SSL *ssl);
// Function prototypes
void start_login( SSL *ssl);
// Retrieve name and password as a cJSON object for transmission
cJSON* get_name_password(void);

