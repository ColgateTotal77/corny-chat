#include "GTK.h"

// Callback to toggle password visibility
static void on_eye_button_clicked(GtkToggleButton *button, gpointer user_data) {
    GtkEntry *password_entry = GTK_ENTRY(user_data);
    gboolean active = gtk_toggle_button_get_active(button);

    // Toggle password visibility
    gtk_entry_set_visibility(password_entry, active);

    // Change the icon of the eye button
    if (active) {
        gtk_button_set_icon_name(GTK_BUTTON(button), "view-reveal-symbolic");
    } else {
        gtk_button_set_icon_name(GTK_BUTTON(button), "view-conceal-symbolic");
    }
}

// Find the most similar email based on the prefix
static const char *find_similar_email(const char *input, GtkListBox *list_box) {
    if (!input || !list_box) {
        return NULL;
    }

    GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(list_box));
    const char *best_match = NULL;

    while (child) {
        if (GTK_IS_LIST_BOX_ROW(child)) {
            GtkWidget *row_child = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child));
            if (GTK_IS_LABEL(row_child)) {  // Check if it's directly a label
                const char *row_text = gtk_label_get_text(GTK_LABEL(row_child));
                
                // If the input is a prefix of the current row text
                if (row_text && g_str_has_prefix(row_text, input)) {
                    best_match = row_text;
                    break;  // Found a match, exit loop
                }
            }
        }
        child = gtk_widget_get_next_sibling(child);
    }

    return best_match;
}

// Key pressed callback for autocomplete
static void on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    (void)keycode;
    (void)state;

    if (keyval == GDK_KEY_Tab) {
        GtkEntry *entry = GTK_ENTRY(user_data);
        GtkListBox *list_box = GTK_LIST_BOX(g_object_get_data(G_OBJECT(controller), "list-box"));
        
        if (!entry || !list_box) {
            return;
        }

        const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));
        if (!current_text) {
            return;
        }

        const char *suggested_text = find_similar_email(current_text, list_box);
        if (suggested_text) {
            gtk_editable_set_text(GTK_EDITABLE(entry), suggested_text);
            // Set cursor to end of text
            gtk_editable_set_position(GTK_EDITABLE(entry), -1);
        }
    }
}

// Callback to handle row selection and set the email in the delete input field
static void on_login_row_selected(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
    (void)list_box; // Suppress unused parameter warning

    // Use delete_entry instead of login_entry
    GtkEntry *delete_entry = GTK_ENTRY(user_data);

    if (row) {
        GtkWidget *child = gtk_list_box_row_get_child(row);
        if (GTK_IS_LABEL(child)) {
            const char *login_text = gtk_label_get_text(GTK_LABEL(child));
            gtk_editable_set_text(GTK_EDITABLE(delete_entry), login_text);
        }
    }
}

static void on_create_button_clicked(GtkButton *button, gpointer user_data) {
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
    GtkWidget *error_label = GTK_WIDGET(entries[2]);
    GtkWidget *success_label = GTK_WIDGET(entries[5]);
    GTK_data_t *GTK_data = (GTK_data_t*)entries[3];
    SSL *ssl = GTK_data->call_data->ssl;

    // Validate individual widgets
    if (!GTK_IS_WIDGET(login_entry) || !GTK_IS_WIDGET(password_entry) || 
        !GTK_IS_WIDGET(error_label) || !GTK_IS_WIDGET(success_label)) {
        fprintf(stderr, "Error: One or more widgets are invalid.\n");
        return;
    }

    // Get text from input fields
    const char *login_input = gtk_editable_get_text(GTK_EDITABLE(login_entry));
    const char *password_input = gtk_editable_get_text(GTK_EDITABLE(password_entry));

    if (!login_input || !*login_input || !password_input || !*password_input) {
        gtk_label_set_text(GTK_LABEL(error_label), "Error: Login and password cannot be empty.");
        gtk_widget_add_css_class(error_label, "error-label");
        gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
        return;
    }

    // Validate login and password
    if (strlen(login_input) < 2 || strlen(login_input) > 30) {
        gtk_label_set_text(GTK_LABEL(error_label), "Error: Login must be 2-30 characters.");
        gtk_widget_add_css_class(error_label, "error-label");
        gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
        return;
    }
    if (!check_password((char *)password_input)) {
        gtk_label_set_text(GTK_LABEL(error_label), "Error: Invalid password. Must be 8-20 characters.");
        gtk_widget_add_css_class(error_label, "error-label");
        gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
        return;
    }

    // Clear error label
    gtk_label_set_text(GTK_LABEL(error_label), "");
    gtk_widget_remove_css_class(error_label, "error-label");

    // Send data to server
    create_new_user(ssl, (char *)login_input, (char *)password_input);

    // Simulated response handling (replace this with actual response logic)
    bool user_created_successfully = true; // Replace with actual server response check

    if (user_created_successfully) {
        // Success message
        gtk_label_set_text(GTK_LABEL(success_label), "Account successfully created!");
        gtk_widget_add_css_class(success_label, "success-label");

        // Clear input fields
        gtk_editable_set_text(GTK_EDITABLE(login_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(password_entry), "");
    } else {
        gtk_label_set_text(GTK_LABEL(error_label), "Error: Account creation failed.");
        gtk_widget_add_css_class(error_label, "error-label");
        gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
    }
}

// static void on_delete_button_clicked(GtkButton *button, gpointer user_data) {
//     (void)button;

//     // Validate user_data
//     if (!user_data) {
//         fprintf(stderr, "Error: user_data is NULL.\n");
//         return;
//     }

//     // Unpack entries
//     gpointer *entries = (gpointer *)user_data;
//     GtkWidget *delete_entry = GTK_WIDGET(entries[6]);
//     GtkWidget *error_label = GTK_WIDGET(entries[2]);
//     GtkWidget *success_label = GTK_WIDGET(entries[5]);
//     GTK_data_t *GTK_data = (GTK_data_t*)entries[3];
//     SSL *ssl = GTK_data->call_data->ssl;

//     // Validate individual widgets
//     if (!GTK_IS_WIDGET(delete_entry) || !GTK_IS_WIDGET(error_label) || !GTK_IS_WIDGET(success_label)) {
//         fprintf(stderr, "Error: One or more widgets are invalid.\n");
//         return;
//     }

//     // Get text from delete input field
//     const char *delete_input = gtk_editable_get_text(GTK_EDITABLE(delete_entry));

//     if (!delete_input || !*delete_input) {
//         gtk_label_set_text(GTK_LABEL(error_label), "Error: Login to delete cannot be empty.");
//         gtk_widget_add_css_class(error_label, "error-label");
//         gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
//         return;
//     }

//     // Clear error label
//     gtk_label_set_text(GTK_LABEL(error_label), "");
//     gtk_widget_remove_css_class(error_label, "error-label");

//     // Send data to server to delete user
//     delete_user(ssl, (char *)delete_input);

//     // Simulated response handling (replace this with actual response logic)
//     bool user_deleted_successfully = true; // Replace with actual server response check

//     if (user_deleted_successfully) {
//         // Success message
//         gtk_label_set_text(GTK_LABEL(success_label), "Account successfully deleted!");
//         gtk_widget_add_css_class(success_label, "success-label");

//         // Clear input field
//         gtk_editable_set_text(GTK_EDITABLE(delete_entry), "");
//     } else {
//         gtk_label_set_text(GTK_LABEL(error_label), "Error: Account deletion failed.");
//         gtk_widget_add_css_class(error_label, "error-label");
//         gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
//     }
// }

// Callback for changing password
// static void on_change_password_button_clicked(GtkButton *button, gpointer user_data) {
//     (void)button;

//     // Validate user_data
//     if (!user_data) {
//         fprintf(stderr, "Error: user_data is NULL.\n");
//         return;
//     }

//     // Unpack entries
//     gpointer *entries = (gpointer *)user_data;
//     GtkWidget *old_password_entry = GTK_WIDGET(entries[7]);
//     GtkWidget *new_password_entry_1 = GTK_WIDGET(entries[8]);
//     GtkWidget *new_password_entry_2 = GTK_WIDGET(entries[9]);
//     GtkWidget *error_label = GTK_WIDGET(entries[2]);
//     GtkWidget *success_label = GTK_WIDGET(entries[5]);
//     GTK_data_t *GTK_data = (GTK_data_t*)entries[3];
//     SSL *ssl = GTK_data->call_data->ssl;

//     // Validate individual widgets
//     if (!GTK_IS_WIDGET(old_password_entry) || !GTK_IS_WIDGET(new_password_entry_1) || !GTK_IS_WIDGET(new_password_entry_2) ||
//         !GTK_IS_WIDGET(error_label) || !GTK_IS_WIDGET(success_label)) {
//         fprintf(stderr, "Error: One or more widgets are invalid.\n");
//         return;
//     }

//     // Get text from input fields
//     const char *old_password_input = gtk_editable_get_text(GTK_EDITABLE(old_password_entry));
//     const char *new_password_input_1 = gtk_editable_get_text(GTK_EDITABLE(new_password_entry_1));
//     const char *new_password_input_2 = gtk_editable_get_text(GTK_EDITABLE(new_password_entry_2));

//     // Check if fields are empty
//     if (!old_password_input || !*old_password_input || !new_password_input_1 || !*new_password_input_1 || !new_password_input_2 || !*new_password_input_2) {
//         gtk_label_set_text(GTK_LABEL(error_label), "Error: All password fields must be filled.");
//         gtk_widget_add_css_class(error_label, "error-label");
//         gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
//         return;
//     }

//     // Check if new passwords match
//     if (strcmp(new_password_input_1, new_password_input_2) != 0) {
//         gtk_label_set_text(GTK_LABEL(error_label), "Error: New passwords do not match.");
//         gtk_widget_add_css_class(error_label, "error-label");
//         gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
//         return;
//     }

//     // Check if old password matches the one in the database
//     if (!is_valid_password(GTK_data->call_data->general_data->db, GTK_data->call_data->client_data->user_data->login, (char *)old_password_input)) {
//         gtk_label_set_text(GTK_LABEL(error_label), "Error: Old password is incorrect.");
//         gtk_widget_add_css_class(error_label, "error-label");
//         gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
//         return;
//     }

//     // Update password in the database
//     cJSON *json = handle_change_password(GTK_data->call_data, cJSON_CreateString(new_password_input_1));

//     bool password_changed_successfully = cJSON_IsBool(cJSON_GetObjectItem(json, "success")) && cJSON_IsTrue(cJSON_GetObjectItem(json, "success"));
//     cJSON_Delete(json);

//     if (password_changed_successfully) {
//         // Success message
//         gtk_label_set_text(GTK_LABEL(success_label), "Password successfully changed!");
//         gtk_widget_add_css_class(success_label, "success-label");

//         // Clear input fields
//         gtk_editable_set_text(GTK_EDITABLE(old_password_entry), "");
//         gtk_editable_set_text(GTK_EDITABLE(new_password_entry_1), "");
//         gtk_editable_set_text(GTK_EDITABLE(new_password_entry_2), "");
//     } else {
//         gtk_label_set_text(GTK_LABEL(error_label), "Error: Password change failed.");
//         gtk_widget_add_css_class(error_label, "error-label");
//         gtk_label_set_text(GTK_LABEL(success_label), ""); // Clear success message
//     }
// }

// Update the login list with users from the server
// static void update_login_list(GtkListBox *login_list, SSL *ssl) {
//     // Clear existing list items
//     GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(login_list));
//     while (child) {
//         GtkWidget *next = gtk_widget_get_next_sibling(child);
//         gtk_list_box_remove(GTK_LIST_BOX(login_list), child);
//         child = next;
//     }

//     // Request all users from the server
//     get_all_clients_userslist(ssl);

//     // Simulated response for example purposes
//     const char *response = "{\"success\":true,\"number_of_users\":1,\"users\":[{\"id\":1,\"active\":true,\"unread_messages\":-1,\"nickname\":\"FirstAdmin\",\"online\":false}],\"command_code\":17}";

//     // Parse the JSON response
//     cJSON *json = cJSON_Parse(response);
//     if (json == NULL) {
//         fprintf(stderr, "Error: Failed to parse JSON response.\n");
//         return;
//     }

//     // Check if the response indicates success
//     cJSON *success = cJSON_GetObjectItem(json, "success");
//     if (!cJSON_IsBool(success) || !cJSON_IsTrue(success)) {
//         cJSON_Delete(json);
//         fprintf(stderr, "Error: Server response indicates failure.\n");
//         return;
//     }

//     // Get the users array
//     cJSON *users = cJSON_GetObjectItem(json, "users");
//     if (!cJSON_IsArray(users)) {
//         cJSON_Delete(json);
//         fprintf(stderr, "Error: Users list is not an array.\n");
//         return;
//     }

//     // Iterate over the users and add them to the login list
//     cJSON *user = NULL;
//     cJSON_ArrayForEach(user, users) {
//         cJSON *active = cJSON_GetObjectItem(user, "active");
//         cJSON *nickname = cJSON_GetObjectItem(user, "nickname");

//         if (cJSON_IsBool(active) && cJSON_IsTrue(active) && cJSON_IsString(nickname)) {
//             GtkWidget *row = gtk_list_box_row_new();
//             GtkWidget *label = gtk_label_new(nickname->valuestring);
//             gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
//             gtk_list_box_append(login_list, row);
//         }
//     }

//     cJSON_Delete(json);
// }

static void gtk_window_close_wrapper(gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    // Apply the CSS styling
    profile_css(GTK_data->window);

    // Close the window
    gtk_window_close(GTK_WINDOW(GTK_data->profile_window));
}

static void activate(GtkApplication *app, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t *)user_data; 
    SSL *ssl = GTK_data->call_data->ssl;            

    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Profile Form");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    GTK_data->profile_window = window;
    
    // Main box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_start(main_box, 30);
    gtk_widget_set_margin_end(main_box, 30);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);

    // Back button with SVG icon
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    GtkWidget *back_button = gtk_button_new();
    gtk_widget_add_css_class(back_button, "back-button");

    // Load SVG icon
    GtkWidget *back_icon = gtk_image_new_from_file("src/chat_visual/images/back.svg");
    gtk_button_set_child(GTK_BUTTON(back_button), back_icon);

    g_signal_connect_swapped(back_button, "clicked", G_CALLBACK(gtk_window_close_wrapper), GTK_data);

    GtkWidget *name_label = gtk_label_new("Vladyslav Zaplitnyi");
    gtk_widget_add_css_class(name_label, "header-label");
    gtk_box_append(GTK_BOX(header_box), back_button);
    gtk_box_append(GTK_BOX(header_box), name_label);

    // Nickname update section
    GtkWidget *nickname_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name(nickname_box, "nickname-box");

    GtkWidget *nickname_input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *nickname_label = gtk_label_new("Enter new nickname:");
    gtk_widget_set_halign(nickname_label, GTK_ALIGN_START);
    GtkWidget *nickname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(nickname_entry), "Enter nickname");
    GtkWidget *update_button = gtk_button_new_with_label("Update");
    gtk_widget_add_css_class(update_button, "update-button");

    gtk_box_append(GTK_BOX(nickname_input_box), nickname_entry);
    gtk_box_append(GTK_BOX(nickname_input_box), update_button);
    gtk_box_append(GTK_BOX(nickname_box), nickname_label);
    gtk_box_append(GTK_BOX(nickname_box), nickname_input_box);

    // Right side form
    GtkWidget *right_form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(right_form, 50);

    // Login input
    GtkWidget *login_label = gtk_label_new("Login:");
    gtk_widget_set_halign(login_label, GTK_ALIGN_START);
    GtkWidget *login_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_entry), "Enter login");

    // Password input with eye icon
    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);

    // Password input with eye icon inside the input box
    GtkWidget *password_box = gtk_overlay_new();
    gtk_widget_set_halign(password_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(password_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(password_box, 10);

    // Password entry field
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_widget_set_hexpand(password_entry, TRUE);

    // Add a custom CSS class to the password entry
    gtk_widget_add_css_class(password_entry, "password-entry");

    // Add the password entry to the overlay as the main child
    gtk_overlay_set_child(GTK_OVERLAY(password_box), password_entry);

    // Eye icon button
    GtkWidget *eye_button = gtk_toggle_button_new();
    gtk_button_set_icon_name(GTK_BUTTON(eye_button), "view-conceal-symbolic");
    gtk_widget_set_tooltip_text(eye_button, "Show/Hide Password");
    gtk_widget_set_size_request(eye_button, 24, 24);
    gtk_widget_add_css_class(eye_button, "eye-icon");
    g_signal_connect(eye_button, "toggled", G_CALLBACK(on_eye_button_clicked), password_entry);

    // Add the eye button to the overlay
    gtk_overlay_add_overlay(GTK_OVERLAY(password_box), eye_button);

    // Align the eye button inside the entry box
    gtk_widget_set_halign(eye_button, GTK_ALIGN_END);
    gtk_widget_set_valign(eye_button, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_end(eye_button, 10);

    // Admin checkbox and create button
    GtkWidget *bottom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *admin_check = gtk_check_button_new_with_label("Admin");
    gtk_widget_add_css_class(admin_check, "admin-checkbox");
    GtkWidget *create_button = gtk_button_new_with_label("Create");
    gtk_widget_add_css_class(create_button, "create-button");

    // Delete user section
    GtkWidget *delete_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *delete_input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *delete_label = gtk_label_new("Enter login to delete:");
    gtk_widget_set_halign(delete_label, GTK_ALIGN_START);
    GtkWidget *delete_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(delete_entry), "Enter login");
    GtkWidget *delete_button = gtk_button_new_with_label("Delete");
    gtk_widget_add_css_class(delete_button, "delete-button");

    gtk_box_append(GTK_BOX(delete_input_box), delete_entry);
    gtk_box_append(GTK_BOX(delete_input_box), delete_button);
    gtk_box_append(GTK_BOX(delete_box), delete_label);
    gtk_box_append(GTK_BOX(delete_box), delete_input_box);

    // Login list background container
    GtkWidget *login_list_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Scrollable login list container
    GtkWidget *scroll_container = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_container), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll_container, -1, 200);
    gtk_widget_add_css_class(scroll_container, "login-list-background");

   // Login list
    GtkWidget *login_list = gtk_list_box_new();
    gtk_widget_set_visible(login_list, TRUE);  // Make sure it's visible
    gtk_widget_set_vexpand(login_list, TRUE);  // Allow vertical expansion
    gtk_widget_set_hexpand(login_list, TRUE);  // Allow horizontal expansion
    gtk_widget_add_css_class(login_list, "login-list");

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_container), login_list);
    GTK_data->profile_data->login_list = GTK_LIST_BOX(login_list);

    // Connect signal for row selection to delete_entry only
    g_signal_connect(login_list, "row-selected", G_CALLBACK(on_login_row_selected), delete_entry);

    gtk_box_append(GTK_BOX(login_list_background), scroll_container);

    // Attach key controller to login entry
    GtkEventController *key_controller = gtk_event_controller_key_new();
    g_object_set_data(G_OBJECT(key_controller), "list-box", login_list);
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_key_pressed), delete_entry);
    gtk_widget_add_controller(delete_entry, key_controller);

    // Update the login list with users from the server
    //update_login_list(GTK_LIST_BOX(login_list), ssl);

    // Send command to get users list
    cJSON *command = cJSON_CreateObject();
    cJSON_AddNumberToObject(command, "command_code", 17);
    send_and_delete_json(ssl, &command);

    // Get response and parse it
    char *response = NULL;
    int bytes_received = recieve_next_response(ssl, &response);
    if (bytes_received > 0) {
        cJSON *parsed_json = cJSON_Parse(response);
        if (parsed_json) {
            update_login_list(GTK_LIST_BOX(login_list), parsed_json);
            // Note: Don't delete parsed_json here as it's deleted in update_login_list
        }
        free(response);
    }

    gtk_box_append(GTK_BOX(bottom_box), admin_check);
    gtk_box_append(GTK_BOX(bottom_box), create_button);

    // Pack right side
    gtk_box_append(GTK_BOX(right_form), login_label);
    gtk_box_append(GTK_BOX(right_form), login_entry);
    gtk_box_append(GTK_BOX(right_form), password_label);
    gtk_box_append(GTK_BOX(right_form), password_box);
    gtk_box_append(GTK_BOX(right_form), bottom_box);
    gtk_box_append(GTK_BOX(right_form), delete_box);
    gtk_box_append(GTK_BOX(right_form), login_list_background);

    // Container for labels (error and success messages)
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Error label for displaying login/password issues
    GtkWidget *error_label = gtk_label_new("");
    gtk_widget_add_css_class(error_label, "error-label");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(message_box), error_label);

    // Success label for displaying successful account creation
    GtkWidget *success_label = gtk_label_new("");
    gtk_widget_add_css_class(success_label, "success-label");
    gtk_widget_set_halign(success_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(message_box), success_label);


    // Prepare data for the callbacks
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 10);
    entries[0] = login_entry;
    entries[1] = password_entry;
    entries[2] = error_label;
    entries[3] = user_data;
    entries[4] = window;
    entries[5] = success_label;
    // entries[6] = delete_entry;
    // entries[7] = old_password_entry;
    // entries[8] = new_password_entry_1;
    // entries[9] = new_password_entry_2;
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_button_clicked), entries);
    //g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), entries);
    //g_signal_connect(change_password_button, "clicked", G_CALLBACK(on_change_password_button_clicked), entries);
    g_signal_connect(login_list, "row-selected", G_CALLBACK(on_login_row_selected), delete_entry);

    // Layout construction
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);

    gtk_box_append(GTK_BOX(left_box), nickname_box);

    gtk_box_append(GTK_BOX(content_box), left_box);
    gtk_box_append(GTK_BOX(content_box), right_form);

    gtk_box_append(GTK_BOX(main_box), header_box);
    gtk_box_append(GTK_BOX(main_box), content_box);
    gtk_box_append(GTK_BOX(main_box), message_box);
    
    //get_all_clients_userslist(ssl);

    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Load CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "src/chat_visual/gtk_src/profile_form.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    gtk_window_present(GTK_WINDOW(window));
}

void profile_start(GTK_data_t *GTK_data) {
    GtkApplication *app = gtk_application_new("com.example.ProfileForm", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), GTK_data);
    g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
}
