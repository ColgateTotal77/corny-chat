#include "GTK.h"

void update_nickname_in_header(GTK_data_t *GTK_data, const char *new_nickname) {
    if (!GTK_data || !GTK_data->profile_data || !GTK_data->profile_data->name_label || !new_nickname) {
        fprintf(stderr, "Error: Missing GTK_data, profile_data, name_label, or new_nickname.\n");
        return;
    }

    // Free the old username memory if it was dynamically allocated
    if (GTK_data->username) {
        free(GTK_data->username);
        GTK_data->username = NULL; // Prevent double free
    }

    // Allocate new memory for the username
    GTK_data->username = strdup(new_nickname);
    if (!GTK_data->username) {
        fprintf(stderr, "Error: Failed to allocate memory for username.\n");
        return;
    }

    // Update the label in the header-box
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->name_label), new_nickname);

    printf("Nickname updated to: %s\n", new_nickname); // Debugging message to confirm the change
}
