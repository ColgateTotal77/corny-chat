#include "GTK.h"

const char *stack_children[] = {"create", "change_password", "delete"};
int current_index = 0;

void on_next_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GtkStack *stack = GTK_STACK(user_data);

    // Ensure the stack is properly initialized
    if (!stack || current_index < 0) {
        return;
    }

    // Move to the next child
    current_index = (current_index + 1) % G_N_ELEMENTS(stack_children);
    gtk_stack_set_visible_child_name(stack, stack_children[current_index]);
}

void on_prev_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GtkStack *stack = GTK_STACK(user_data);

    // Ensure the stack is properly initialized
    if (!stack || current_index < 0) {
        return;
    }

    // Move to the previous child
    current_index = (current_index - 1 + G_N_ELEMENTS(stack_children)) % G_N_ELEMENTS(stack_children);
    gtk_stack_set_visible_child_name(stack, stack_children[current_index]);
}

static void apply_css(void) {
    GtkCssProvider *css_provider = gtk_css_provider_new();

    // Load CSS with only two parameters
    gtk_css_provider_load_from_path(css_provider, "src/chat_visual/gtk_src/GTK_Start/style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(css_provider);
}

// Callback for the "Deactivate" button
// void on_deactivate_button_clicked(GtkButton *button, gpointer user_data) {
//     (void)button;
//     GTK_data_t *GTK_data = (GTK_data_t *)user_data;
//     GtkEntry *deactivate_entry = GTK_ENTRY(GTK_data->profile_data->deactivate_entry);
//     GtkLabel *error_label = GTK_LABEL(GTK_data->profile_data->deactivate_error_label);
//     SSL *ssl = GTK_data->call_data->ssl;

//     // Retrieve the text from the entry
//     const char *user_id_str = gtk_editable_get_text(GTK_EDITABLE(deactivate_entry));

//     // Validate the input
//     if (!user_id_str || strlen(user_id_str) == 0) {
//         gtk_label_set_text(error_label, "User ID cannot be empty.");
//         gtk_widget_set_visible(GTK_WIDGET(error_label), TRUE);
//         return;
//     }

//     // Convert the text to an integer
//     char *endptr;
//     int user_id = strtol(user_id_str, &endptr, 10);
//     if (*endptr != '\0' || user_id <= 0) {
//         gtk_label_set_text(error_label, "Invalid User ID. Please enter a valid number.");
//         gtk_widget_set_visible(GTK_WIDGET(error_label), TRUE);
//         return;
//     }

//     // Send the deactivation request to the server
//     deactivate_user(ssl, user_id);

//     // Provide feedback to the user
//     gtk_label_set_text(error_label, "Deactivation request sent.");
//     gtk_widget_set_visible(GTK_WIDGET(error_label), TRUE);
// }

static void gtk_window_close_wrapper(gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    // Apply the CSS styling
    apply_css();

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

    // Create Stack for Carousel
    GtkWidget *carousel_stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(carousel_stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(carousel_stack), 300);

    // Declare a label for the current window name
    GtkWidget *current_window_label = gtk_label_new("Create a new profile"); // Default title
    gtk_widget_add_css_class(current_window_label, "current-window-label");
    
    // Header box with Back button, title, and navigation buttons
    GtkWidget *header_box_with_nav = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);

    // Left part of header (Back button and title)
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    GtkWidget *back_button = gtk_button_new();
    gtk_widget_add_css_class(back_button, "back-button");
    GtkWidget *back_icon = gtk_image_new_from_file("src/chat_visual/images/back.svg");
    gtk_button_set_child(GTK_BUTTON(back_button), back_icon);
    g_signal_connect_swapped(back_button, "clicked", G_CALLBACK(gtk_window_close_wrapper), GTK_data);

    // // Add a label for the username
    GtkWidget *name_label = gtk_label_new(GTK_data->username ? GTK_data->username : "Unknown User");
    gtk_widget_add_css_class(name_label, "header-label");

    // Store the name_label in GTK_data for later updates
    GTK_data->profile_data->name_label = GTK_WIDGET(name_label);

    // Add the name_label to the header-box
    gtk_box_append(GTK_BOX(header_box), back_button);
    gtk_box_append(GTK_BOX(header_box), name_label);

    // Right part of header (Back and Forward icons)
    GtkWidget *nav_buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10); // Declare and initialize nav_buttons_box

    // Back Button with Icon
    GtkWidget *prev_button = gtk_button_new();
    GtkWidget *prev_icon = gtk_image_new_from_file("src/chat_visual/images/back_arrow.svg");
    gtk_button_set_child(GTK_BUTTON(prev_button), prev_icon);
    gtk_widget_add_css_class(prev_button, "nav-button");

    // Forward Button with Icon
    GtkWidget *next_button = gtk_button_new();
    GtkWidget *next_icon = gtk_image_new_from_file("src/chat_visual/images/forward_arrow.svg");
    gtk_button_set_child(GTK_BUTTON(next_button), next_icon);
    gtk_widget_add_css_class(next_button, "nav-button");

    // Connect Navigation Buttons
    g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_button_clicked), carousel_stack);
    g_signal_connect(prev_button, "clicked", G_CALLBACK(on_prev_button_clicked), carousel_stack);

    // Append Buttons to Navigation Box
    gtk_box_append(GTK_BOX(nav_buttons_box), prev_button);
    gtk_box_append(GTK_BOX(nav_buttons_box), next_button);

    // Combine header box and nav buttons box in the main header container
    gtk_box_append(GTK_BOX(header_box_with_nav), header_box);
    gtk_widget_set_halign(nav_buttons_box, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(header_box_with_nav), nav_buttons_box);

    // Add header_with_nav to the main_box
    gtk_box_append(GTK_BOX(main_box), header_box_with_nav);
        

    // Left Box (Nickname and Password Sections)
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);

    // Nickname Section
    GtkWidget *nickname_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *nickname_label = gtk_label_new("Enter new nickname:");
    gtk_widget_set_halign(nickname_label, GTK_ALIGN_START);

    GtkWidget *nickname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(nickname_entry), "Enter nickname");
    GTK_data->profile_data->nickname_entry = nickname_entry; // Store in profile_data

    GtkWidget *update_button = gtk_button_new_with_label("Update");
    gtk_widget_add_css_class(update_button, "update-button");

    GtkWidget *nickname_error_label = gtk_label_new("");
    gtk_widget_add_css_class(nickname_error_label, "error-label");
    gtk_widget_set_halign(nickname_error_label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(nickname_error_label, 2); // Reduce margin from the top
    gtk_widget_set_margin_bottom(nickname_error_label, 0); // Optional: Remove bottom margin

    // Store the error label in profile_data
    GTK_data->profile_data->nickname_error_label = nickname_error_label;

    // Connect the "Update" button signal
    g_signal_connect(update_button, "clicked", G_CALLBACK(on_update_button_clicked), GTK_data);

    GtkWidget *nickname_input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(nickname_input_box), nickname_entry);
    gtk_box_append(GTK_BOX(nickname_input_box), update_button);

    gtk_box_append(GTK_BOX(nickname_box), nickname_label);
    gtk_box_append(GTK_BOX(nickname_box), nickname_input_box);
    gtk_box_append(GTK_BOX(nickname_box), nickname_error_label); // Add error label to the box

    GtkWidget *nickname_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_margin_top(nickname_separator, 10);
    gtk_widget_set_margin_bottom(nickname_separator, 10);
    gtk_box_append(GTK_BOX(nickname_box), nickname_separator);

    gtk_box_append(GTK_BOX(left_box), nickname_box);

    // Change Password Section
    GtkWidget *change_password_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Old Password
    GtkWidget *old_password_label = gtk_label_new("Enter old password:");
    gtk_widget_set_halign(old_password_label, GTK_ALIGN_START);

    // Old password input with eye button
    GtkWidget *old_password_box = gtk_overlay_new();
    GtkWidget *old_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(old_password_entry), "Enter old password");
    gtk_entry_set_visibility(GTK_ENTRY(old_password_entry), FALSE);
    gtk_widget_add_css_class(old_password_entry, "password-entry");
    gtk_overlay_set_child(GTK_OVERLAY(old_password_box), old_password_entry);

    GtkWidget *old_eye_button = gtk_toggle_button_new();
    gtk_button_set_icon_name(GTK_BUTTON(old_eye_button), "view-conceal-symbolic");
    gtk_widget_set_tooltip_text(old_eye_button, "Show/Hide Password");
    gtk_widget_set_size_request(old_eye_button, 24, 24);
    gtk_widget_add_css_class(old_eye_button, "eye-icon");
    gtk_overlay_add_overlay(GTK_OVERLAY(old_password_box), old_eye_button);
    gtk_widget_set_halign(old_eye_button, GTK_ALIGN_END);
    gtk_widget_set_valign(old_eye_button, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_end(old_eye_button, 10);

    // Connect the eye button for old password
    g_signal_connect(old_eye_button, "toggled", G_CALLBACK(on_eye_button_clicked), old_password_entry);

    // New Password 1
    GtkWidget *new_password_label_1 = gtk_label_new("Enter new password:");
    gtk_widget_set_halign(new_password_label_1, GTK_ALIGN_START);

    // New password input with eye button
    GtkWidget *new_password_box_1 = gtk_overlay_new();
    GtkWidget *new_password_entry_1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_password_entry_1), "Enter new password");
    gtk_entry_set_visibility(GTK_ENTRY(new_password_entry_1), FALSE);
    gtk_widget_add_css_class(new_password_entry_1, "password-entry");
    gtk_overlay_set_child(GTK_OVERLAY(new_password_box_1), new_password_entry_1);

    GtkWidget *new_eye_button_1 = gtk_toggle_button_new();
    gtk_button_set_icon_name(GTK_BUTTON(new_eye_button_1), "view-conceal-symbolic");
    gtk_widget_set_tooltip_text(new_eye_button_1, "Show/Hide Password");
    gtk_widget_set_size_request(new_eye_button_1, 24, 24);
    gtk_widget_add_css_class(new_eye_button_1, "eye-icon");
    gtk_overlay_add_overlay(GTK_OVERLAY(new_password_box_1), new_eye_button_1);
    gtk_widget_set_halign(new_eye_button_1, GTK_ALIGN_END);
    gtk_widget_set_valign(new_eye_button_1, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_end(new_eye_button_1, 10);

    // Connect the eye button for new password 1
    g_signal_connect(new_eye_button_1, "toggled", G_CALLBACK(on_eye_button_clicked), new_password_entry_1);

    // New Password 2
    GtkWidget *new_password_label_2 = gtk_label_new("Re-enter new password:");
    gtk_widget_set_halign(new_password_label_2, GTK_ALIGN_START);

    // New password re-entry input with eye button
    GtkWidget *new_password_box_2 = gtk_overlay_new();
    GtkWidget *new_password_entry_2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_password_entry_2), "Re-enter new password");
    gtk_entry_set_visibility(GTK_ENTRY(new_password_entry_2), FALSE);
    gtk_widget_add_css_class(new_password_entry_2, "password-entry");
    gtk_overlay_set_child(GTK_OVERLAY(new_password_box_2), new_password_entry_2);

    GtkWidget *new_eye_button_2 = gtk_toggle_button_new();
    gtk_button_set_icon_name(GTK_BUTTON(new_eye_button_2), "view-conceal-symbolic");
    gtk_widget_set_tooltip_text(new_eye_button_2, "Show/Hide Password");
    gtk_widget_set_size_request(new_eye_button_2, 24, 24);
    gtk_widget_add_css_class(new_eye_button_2, "eye-icon");
    gtk_overlay_add_overlay(GTK_OVERLAY(new_password_box_2), new_eye_button_2);
    gtk_widget_set_halign(new_eye_button_2, GTK_ALIGN_END);
    gtk_widget_set_valign(new_eye_button_2, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_end(new_eye_button_2, 10);

    // Connect the eye button for new password 2
    g_signal_connect(new_eye_button_2, "toggled", G_CALLBACK(on_eye_button_clicked), new_password_entry_2);

    GtkWidget *change_password_button = gtk_button_new_with_label("Change Password");
    gtk_widget_add_css_class(change_password_button, "change-password-button");
    // Append widgets to the box
    gtk_box_append(GTK_BOX(change_password_box), old_password_label);
    gtk_box_append(GTK_BOX(change_password_box), old_password_box);
    gtk_box_append(GTK_BOX(change_password_box), new_password_label_1);
    gtk_box_append(GTK_BOX(change_password_box), new_password_box_1);
    gtk_box_append(GTK_BOX(change_password_box), new_password_label_2);
    gtk_box_append(GTK_BOX(change_password_box), new_password_box_2);
    gtk_box_append(GTK_BOX(change_password_box), change_password_button);

    // Assigning widgets to GTK_data->profile_data
    GTK_data->profile_data->old_password_entry = old_password_entry;
    GTK_data->profile_data->new_password_entry_1 = new_password_entry_1;
    GTK_data->profile_data->new_password_entry_2 = new_password_entry_2;
    GTK_data->profile_data->password_error_label = gtk_label_new("");
    GTK_data->profile_data->password_success_label = gtk_label_new("");

    // Add password error and success labels to the box (Optional)
    gtk_widget_add_css_class(GTK_data->profile_data->password_error_label, "error-label");
    gtk_widget_add_css_class(GTK_data->profile_data->password_success_label, "success-label");
    gtk_box_append(GTK_BOX(change_password_box), GTK_data->profile_data->password_error_label);
    gtk_box_append(GTK_BOX(change_password_box), GTK_data->profile_data->password_success_label);

    // Connect the Change Password button signal
    g_signal_connect(change_password_button, "clicked", G_CALLBACK(on_change_password_button_clicked), GTK_data);


    gtk_box_append(GTK_BOX(left_box), change_password_box);

    // Right Box (Carousel Content)
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);

 
    // Create "Create" Window
    GtkWidget *create_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

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

    // Append widgets to the bottom box
    gtk_box_append(GTK_BOX(bottom_box), admin_check);
    gtk_box_append(GTK_BOX(bottom_box), create_button);

    // Add all widgets to the create_box
    gtk_box_append(GTK_BOX(create_box), login_label);
    gtk_box_append(GTK_BOX(create_box), login_entry);
    gtk_box_append(GTK_BOX(create_box), password_label);
    gtk_box_append(GTK_BOX(create_box), password_box);
    gtk_box_append(GTK_BOX(create_box), bottom_box);

    // Add the Create box to the carousel
    gtk_stack_add_titled(GTK_STACK(carousel_stack), create_box, "create", "Create");

    // Create "Delete" Window
    GtkWidget *delete_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Input field and Delete button
    GtkWidget *delete_label = gtk_label_new("Enter login to deactivate:");
    gtk_widget_set_halign(delete_label, GTK_ALIGN_START);

    GtkWidget *delete_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(delete_entry), "Enter login");

    GtkWidget *delete_button = gtk_button_new_with_label("Deactivation");
    gtk_widget_add_css_class(delete_button, "delete-button");

    GtkWidget *delete_input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    // Ensure widgets have no parent before appending
    if (gtk_widget_get_parent(delete_entry)) {
        gtk_widget_unparent(delete_entry);
    }
    if (gtk_widget_get_parent(delete_button)) {
        gtk_widget_unparent(delete_button);
    }

    // Append widgets to the input box
    gtk_box_append(GTK_BOX(delete_input_box), delete_entry);
    gtk_box_append(GTK_BOX(delete_input_box), delete_button);

    // Add input box to the delete box
    gtk_box_append(GTK_BOX(delete_box), delete_label);
    gtk_box_append(GTK_BOX(delete_box), delete_input_box);

    // Login list background container
    GtkWidget *login_list_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Scrollable login list container
    GtkWidget *scroll_container = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_container), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll_container, -1, 200);
    gtk_widget_add_css_class(scroll_container, "login-list-background");

    GtkWidget *login_list = gtk_list_box_new();
    gtk_widget_set_visible(login_list, TRUE);  // Ensure visibility
    gtk_widget_set_vexpand(login_list, TRUE);  // Allow vertical expansion
    gtk_widget_set_hexpand(login_list, TRUE);  // Allow horizontal expansion
    gtk_widget_add_css_class(login_list, "login-list");

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_container), login_list);
    GTK_data->profile_data->login_list = GTK_LIST_BOX(login_list);

    //update_login_list(GTK_LIST_BOX(login_list), parced_json);

    GtkEventController *key_controller = gtk_event_controller_key_new();
    g_object_set_data(G_OBJECT(key_controller), "list-box", login_list);
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_key_pressed), delete_entry);
    gtk_widget_add_controller(delete_entry, key_controller);

    // Connect signal for row selection to delete_entry only
    g_signal_connect(login_list, "row-selected", G_CALLBACK(on_login_row_selected), delete_entry);
    //g_signal_connect(delete_button, "clicked", G_CALLBACK(on_deactivate_button_clicked), GTK_data);

    // Append scroll container to the background container
    gtk_box_append(GTK_BOX(login_list_background), scroll_container);

    // Add login list background to the delete box
    gtk_box_append(GTK_BOX(delete_box), login_list_background);

    // Add the Delete box to the carousel
    gtk_stack_add_titled(GTK_STACK(carousel_stack), delete_box, "delete", "Delete");

    // Create "Change Password" Window
    GtkWidget *password_box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *password_label1 = gtk_label_new("Change password:");

    GtkWidget *new_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_password_entry), "Enter new password");
    gtk_entry_set_visibility(GTK_ENTRY(new_password_entry), FALSE);

    GtkWidget *password_button = gtk_button_new_with_label("Change Password");
    gtk_box_append(GTK_BOX(password_box1), password_label1);
    gtk_box_append(GTK_BOX(password_box1), new_password_entry);
    gtk_box_append(GTK_BOX(password_box1), password_button);
    gtk_stack_add_titled(GTK_STACK(carousel_stack), password_box1, "change_password", "Change Password");


    // Connect Navigation Buttons
    g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_button_clicked), carousel_stack);
    g_signal_connect(prev_button, "clicked", G_CALLBACK(on_prev_button_clicked), carousel_stack);


    // Add Stack and Navigation to Right Box
    gtk_box_append(GTK_BOX(right_box), carousel_stack);

    // Error label for displaying login/password issues
    GtkWidget *error_label = gtk_label_new("");
    gtk_widget_add_css_class(error_label, "error-label");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(create_box), error_label);

    // Success label for displaying successful account creation
    GtkWidget *success_label = gtk_label_new("");
    gtk_widget_add_css_class(success_label, "success-label");
    gtk_widget_set_halign(success_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(create_box), success_label);

    // Prepare data for the callbacks
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 10);
    entries[0] = login_entry;
    entries[1] = password_entry;
    entries[2] = error_label;
    entries[3] = user_data;
    entries[4] = window;
    entries[5] = success_label;
    entries[6] = admin_check;
    // entries[6] = delete_entry;
    // entries[7] = old_password_entry;
    // entries[8] = new_password_entry_1;
    // entries[9] = new_password_entry_2;
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_button_clicked), entries);
    //g_signal_connect(change_password_button, "clicked", G_CALLBACK(on_change_password_button_clicked), GTK_data);
    //g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), entries);
    //g_signal_connect(change_password_button, "clicked", G_CALLBACK(on_change_password_button_clicked), entries);
    //g_signal_connect(login_list, "row-selected", G_CALLBACK(on_login_row_selected), delete_entry);

    // Content Box (Horizontal Split)
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_append(GTK_BOX(content_box), left_box);
    gtk_box_append(GTK_BOX(content_box), right_box);

    // Add header and content box to the main box
    gtk_box_append(GTK_BOX(main_box), header_box);
    gtk_box_append(GTK_BOX(main_box), content_box);

    cJSON *command = cJSON_CreateObject();
    cJSON_AddNumberToObject(command, "command_code", 17);
    send_and_delete_json(ssl, &command);

    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Load CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "src/chat_visual/gtk_src/profile_form/profile_form.css");
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
