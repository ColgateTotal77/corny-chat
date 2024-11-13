#pragma once

#include <gtk/gtk.h>
#include <unistd.h>
#include <client.h>
#include <commands.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "cJSON.h"  // Ensure cJSON is included for the cJSON* return type
#include "client.h"


void GTK_start(SSL *ssl);
// Function prototypes
void start_login( SSL *ssl);
// Retrieve name and password as a cJSON object for transmission
cJSON* get_name_password(void);

