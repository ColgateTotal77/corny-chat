#include "GTK.h"

void on_create_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;

    // Validate user_data
    if (!user_data) {
        fprintf(stderr, "Error: user_data is NULL.\n");
        return;
    }

    // Unpack entries
    gpointer *entries = (gpointer *)user_data;
    GtkWidget *login_entry = GTK_WIDGET(entries[0]);
    GtkWidget *password_entry = GTK_WIDGET(entries[1]);
    GtkWidget *create_error_label = GTK_WIDGET(entries[2]);
    GtkWidget *create_success_label = GTK_WIDGET(entries[5]);
    GtkWidget *admin_check = GTK_WIDGET(entries[6]);
    GTK_data_t *GTK_data = (GTK_data_t *)entries[3];
    SSL *ssl = GTK_data->call_data->ssl;

    // IMPORTANT: Store labels in GTK_data FIRST
    GTK_data->profile_data->create_error_label = create_error_label;
    GTK_data->profile_data->create_success_label = create_success_label;
    GTK_data->profile_data->login_entry = login_entry;
    GTK_data->profile_data->password_entry = password_entry;

    // Validate individual widgets
    if (!GTK_IS_WIDGET(login_entry) || !GTK_IS_WIDGET(password_entry) || 
        !GTK_IS_WIDGET(create_error_label) || !GTK_IS_WIDGET(create_success_label) || 
        !GTK_IS_WIDGET(admin_check)) {
        fprintf(stderr, "Error: One or more widgets are invalid.\n");
        return;
    }

    // Get text from input fields
    const char *login_input = gtk_editable_get_text(GTK_EDITABLE(login_entry));
    const char *password_input = gtk_editable_get_text(GTK_EDITABLE(password_entry));

    // Add debug prints to verify values
    printf("Login input: %s\n", login_input);
    printf("Password input: %s\n", password_input);

    if (!login_input || !*login_input || !password_input || !*password_input) {
        printf("Displaying empty fields error\n");
        display_ui_message(GTK_data, "Error: Login and password cannot be empty.", false);
        return;
    }

    // Validate login and password
    if (strlen(login_input) < 2 || strlen(login_input) > 30) {
        printf("Displaying login length error\n");
        display_ui_message(GTK_data, "Error: Login must be 2-30 characters.", false);
        return;
    }
    
    if (!check_password((char *)password_input)) {
        printf("Displaying password error\n");
        display_ui_message(GTK_data, "Error: Invalid password. Must be 8-20 characters.", false);
        return;
    }

    // Get the state of the admin checkbox
    gboolean is_admin = gtk_check_button_get_active(GTK_CHECK_BUTTON(admin_check));

    // Send data to server
    create_new_user(ssl, (char *)login_input, (char *)password_input, is_admin);
}
