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

void fetch_and_populate_users(SSL *ssl, GtkListBox *login_list) {
    // Send the request to fetch users
    see_all_users(ssl);

    // Buffer to receive the response
    char buffer[4096];
    int bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1);

    if (bytes_received <= 0) {
        fprintf(stderr, "Error: Failed to receive response from server.\n");
        return;
    }

    // Null-terminate the buffer
    buffer[bytes_received] = '\0';

    // Debug: Print the raw server response
    printf("Raw server response: %s\n", buffer);



    // Parse the JSON response
    cJSON *response = cJSON_Parse(buffer);
    if (!response) {
        fprintf(stderr, "Error: Failed to parse server response. Raw: %s\n", buffer);
        return;
    }

    // Check for "success" key
    cJSON *success = cJSON_GetObjectItem(response, "success");
    if (!cJSON_IsBool(success) || !cJSON_IsTrue(success)) {
        fprintf(stderr, "Error: Operation failed. 'success' is not true.\n");
        cJSON_Delete(response);
        return;
    }

    // Extract the `users` array
    cJSON *users_array = cJSON_GetObjectItem(response, "users");


    // Clear the existing `login_list`
    GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(login_list));
    while (child != NULL) {
        GtkWidget *next_child = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next_child;
    }

    // Populate the `login_list` with user logins
    cJSON *user = NULL;
    cJSON_ArrayForEach(user, users_array) {
        // Extract the `login` field
        cJSON *login = cJSON_GetObjectItem(user, "login");
        if (!cJSON_IsString(login)) {
            continue; // Skip invalid entries
        }

        // Create a new row for the login
        GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_widget_set_hexpand(row_box, TRUE);

        GtkWidget *email_label = gtk_label_new(cJSON_GetStringValue(login));
        gtk_widget_set_halign(email_label, GTK_ALIGN_START);
        gtk_widget_set_valign(email_label, GTK_ALIGN_CENTER);
        gtk_widget_set_hexpand(email_label, TRUE);
        gtk_box_append(GTK_BOX(row_box), email_label);

        GtkWidget *row_widget = gtk_list_box_row_new();
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row_widget), row_box);

        gtk_list_box_append(GTK_LIST_BOX(login_list), row_widget);
    }

    // Free the JSON object
    cJSON_Delete(response);

}



// Find the most similar email based on the prefix
static const char *find_similar_email(const char *input, GtkListBox *list_box) {
    GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(list_box));
    const char *email = NULL;

    while (child) {
        GtkWidget *row_child = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child));
        GtkWidget *box_child = gtk_widget_get_first_child(row_child);
        const char *row_text = gtk_label_get_text(GTK_LABEL(box_child));

        if (g_str_has_prefix(row_text, input)) {
            email = row_text;
            break;
        }

        child = gtk_widget_get_next_sibling(child);
    }

    return email;
}

// Key pressed callback for autocomplete
static void on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    (void)keycode;  // Suppress unused parameter warning
    (void)state;    // Suppress unused parameter warning

    GtkEntry *delete_entry = GTK_ENTRY(user_data);
    GtkListBox *login_list = GTK_LIST_BOX(g_object_get_data(G_OBJECT(controller), "list-box"));

    if (keyval == GDK_KEY_Tab) {
        const char *current_text = gtk_editable_get_text(GTK_EDITABLE(delete_entry));
        const char *suggested_email = find_similar_email(current_text, login_list);

        if (suggested_email) {
            gtk_editable_set_text(GTK_EDITABLE(delete_entry), suggested_email);
        }
    }
}

// Callback to handle row selection and set the email in the delete input field
static void on_login_row_selected(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
    (void)list_box; // Suppress unused parameter warning

    GtkEntry *delete_entry = GTK_ENTRY(user_data);

    if (row) {
        GtkWidget *child = gtk_list_box_row_get_child(row);
        GtkWidget *box_child = gtk_widget_get_first_child(child);
        const char *email = gtk_label_get_text(GTK_LABEL(box_child));

        gtk_editable_set_text(GTK_EDITABLE(delete_entry), email);
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
    SSL *ssl = (SSL *)entries[3];

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


static void activate(GtkApplication *app, gpointer ssl) {

    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Profile Form");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

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

    // Login delete section
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
    gtk_widget_add_css_class(login_list, "login-list");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_container), login_list);

    // Connect signal for row selection
    g_signal_connect(login_list, "row-selected", G_CALLBACK(on_login_row_selected), delete_entry);

    fetch_and_populate_users((SSL *)ssl, GTK_LIST_BOX(login_list));


    gtk_box_append(GTK_BOX(login_list_background), scroll_container);

    // Attach key controller to delete entry
    GtkEventControllerKey *key_controller = GTK_EVENT_CONTROLLER_KEY(gtk_event_controller_key_new());
    g_object_set_data(G_OBJECT(key_controller), "list-box", login_list);
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_key_pressed), delete_entry);
    gtk_widget_add_controller(delete_entry, GTK_EVENT_CONTROLLER(key_controller));

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

    // Error label for displaying login/password issues
    GtkWidget *error_label = gtk_label_new("");
    gtk_widget_add_css_class(error_label, "error-label");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(create_button), error_label);

    // Error label for displaying login/password issues
    GtkWidget *success_label = gtk_label_new("");
    gtk_widget_add_css_class(success_label, "success-label");
    gtk_widget_set_halign(success_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(create_button), success_label);

    // Prepare data for the callback
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 7);
    entries[0] = login_entry;
    entries[1] = password_entry;
    entries[2] = error_label;
    entries[3] = ssl;
    entries[4] = window;
    entries[5] = success_label;
    entries[6] = delete_entry;
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_button_clicked), entries);

    gtk_box_append(GTK_BOX(bottom_box), admin_check);
    gtk_box_append(GTK_BOX(bottom_box), create_button);

    // Pack right side
    gtk_box_append(GTK_BOX(right_form), login_label);
    gtk_box_append(GTK_BOX(right_form), login_entry);
    gtk_box_append(GTK_BOX(right_form), password_label);
    gtk_box_append(GTK_BOX(right_form), password_box);
    gtk_box_append(GTK_BOX(right_form), bottom_box);
    gtk_box_append(GTK_BOX(right_form), error_label);

    // Layout construction
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);

    gtk_box_append(GTK_BOX(left_box), nickname_box);
    gtk_box_append(GTK_BOX(left_box), delete_box);
    gtk_box_append(GTK_BOX(left_box), login_list_background);

    gtk_box_append(GTK_BOX(content_box), left_box);
    gtk_box_append(GTK_BOX(content_box), right_form);

    gtk_box_append(GTK_BOX(main_box), header_box);
    gtk_box_append(GTK_BOX(main_box), content_box);

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

void profile_start(SSL *ssl) {
  // Use a static variable to track the application instance
    static GtkApplication *app = NULL;
    
    // If no app exists, create a new one
    if (app == NULL) {
        app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_NON_UNIQUE);
        g_signal_connect(app, "activate", G_CALLBACK(activate), ssl);
    }
    
    // Activate the application
    g_application_run(G_APPLICATION(app), 0, NULL);
    
    // Optional: Uncomment if you want to reset the app after closing
    // g_object_unref(app);
    // app = NULL;
}

