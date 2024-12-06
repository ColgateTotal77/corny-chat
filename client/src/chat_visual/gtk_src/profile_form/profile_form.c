#include "GTK.h"

const char *stack_children[] = {"create", "change_password", "delete", "activate"};
int current_index = 0;

static gboolean is_transitioning = FALSE;

static GtkWidget *current_window_label = NULL;  // Add this global variable

// Fallback function to reset is_transitioning
static gboolean reset_transition_flag(gpointer data) {
    (void)data; // Avoid unused parameter warning
    is_transitioning = FALSE;
    return G_SOURCE_REMOVE; // Stop the timeout
}

// Update the update_window_title function
void update_window_title(const char *stack_name) {
    if (!current_window_label) return;

    const char *title;
    if (g_strcmp0(stack_name, "create") == 0)
        title = "Create New User";
    else if (g_strcmp0(stack_name, "change_password") == 0)
        title = "Change Password";
    else if (g_strcmp0(stack_name, "delete") == 0)
        title = "Deactivate User";
    else if (g_strcmp0(stack_name, "activate") == 0)
        title = "Activate User";
    else
        title = "Profile Form";
        
    gtk_label_set_text(GTK_LABEL(current_window_label), title);
    is_transitioning = TRUE; // Set flag for transition
    g_timeout_add(500, reset_transition_flag, NULL);
}

void on_transition_done(GtkStack *stack, GParamSpec *pspec, gpointer user_data) {
    (void)pspec;
    (void)user_data;

    is_transitioning = FALSE;
    const char *current_child = gtk_stack_get_visible_child_name(stack);
    update_window_title(current_child);
    g_print("Transition completed to: %s\n", current_child);
    is_transitioning = TRUE; // Set flag for transition
    g_timeout_add(500, reset_transition_flag, NULL);
}

void on_next_button_clicked(GtkButton *button, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);

    if (is_transitioning) {
        g_print("Transition already in progress. Ignoring button press.\n");
        return;
    }
    
    is_transitioning = TRUE;
    current_index = (current_index + 1) % G_N_ELEMENTS(stack_children);
    gtk_stack_set_visible_child_name(stack, stack_children[current_index]);
    update_window_title(stack_children[current_index]);

    g_timeout_add(1000, reenable_button_after_delay, button);
    g_timeout_add(500, reset_transition_flag, NULL);
}

void on_prev_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GtkStack *stack = GTK_STACK(user_data);

    if (is_transitioning) {
        g_print("Transition already in progress. Ignoring button press.\n");
        return; // Prevent triggering a new transition
    }

    is_transitioning = TRUE; // Set flag for transition

    // Calculate the previous index
    current_index = (current_index - 1 + G_N_ELEMENTS(stack_children)) % G_N_ELEMENTS(stack_children);
    gtk_stack_set_visible_child_name(stack, stack_children[current_index]);
    update_window_title(stack_children[current_index]);

    // Add a fallback to reset the flag after the transition duration
    g_timeout_add(1000, reenable_button_after_delay, button);
    g_timeout_add(500, reset_transition_flag, NULL);
}

static void gtk_window_close_wrapper(gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    if (GTK_data && GTK_IS_WINDOW(GTK_data->profile_window)) {
        // Cleanup before closing
        current_window_label = NULL; // Reset global variable
        is_transitioning = FALSE;    // Reset transition flag

        gtk_window_close(GTK_WINDOW(GTK_data->profile_window));
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t *)user_data; 
    SSL *ssl = GTK_data->call_data->ssl;

    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Profile Form");
    if (GTK_data->is_admin) {
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    } else {
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    }
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    GTK_data->profile_window = window;

    // To center the window in GTK4
    gtk_window_set_transient_for(GTK_WINDOW(window), NULL);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    
    // Main box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_start(main_box, 30);
    gtk_widget_set_margin_end(main_box, 30);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);
    gtk_widget_set_hexpand(main_box, TRUE);
    gtk_widget_set_vexpand(main_box, TRUE);

    // Create Stack for Carousel
    GtkWidget *carousel_stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(carousel_stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(carousel_stack), 300);

    // Header section
    GtkWidget *header_box = gtk_grid_new();
    gtk_widget_set_size_request(header_box, 800, 60);
    gtk_widget_set_hexpand(header_box, TRUE);
    gtk_widget_set_vexpand(header_box, FALSE);
    gtk_widget_add_css_class(header_box, "header_box");

    // Left side of header (Back button and nickname)
    GtkWidget *header_left_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(header_left_box, GTK_ALIGN_START);
    gtk_widget_set_hexpand(header_left_box, TRUE);

    // Back button
    GtkWidget *back_button = gtk_button_new();
    gtk_widget_add_css_class(back_button, "back-button");
    GtkWidget *back_icon = gtk_image_new_from_file("src/chat_visual/images/back.svg");
    gtk_widget_set_margin_bottom(back_button, 14);
    gtk_button_set_child(GTK_BUTTON(back_button), back_icon);
    g_signal_connect_swapped(back_button, "clicked", G_CALLBACK(gtk_window_close_wrapper), GTK_data);

    // Nickname label
    GtkWidget *name_label = gtk_label_new(GTK_data->username ? GTK_data->username : "Unknown User");
    gtk_widget_add_css_class(name_label, "header-label");
    gtk_widget_set_size_request(name_label, 200, -1); // Fixed width for nickname
    gtk_widget_set_margin_bottom(name_label, 12);
    gtk_label_set_ellipsize(GTK_LABEL(name_label), PANGO_ELLIPSIZE_END); // Ellipsize long nicknames
    GTK_data->profile_data->name_label = GTK_WIDGET(name_label);

    // Add back button and nickname to left box
    gtk_box_append(GTK_BOX(header_left_box), back_button);
    gtk_box_append(GTK_BOX(header_left_box), name_label);

    // Add left box to grid
    gtk_grid_attach(GTK_GRID(header_box), header_left_box, 0, 0, 1, 1);

    if (GTK_data->is_admin) {
        // Right side of header (Navigation buttons and title)
        GtkWidget *header_right_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_halign(header_right_box, GTK_ALIGN_END);
        gtk_widget_add_css_class(header_right_box, "header_right_box");

        // Set the same size as the "Create" button container
        gtk_widget_set_size_request(header_right_box, 400, 60);
        gtk_widget_set_margin_bottom(header_right_box, 14);
        gtk_widget_set_margin_end(header_right_box, 23);

        // Previous button
        GtkWidget *prev_button = gtk_button_new();
        gtk_widget_set_size_request(prev_button, 40, 40);
        GtkWidget *prev_icon = gtk_image_new_from_file("src/chat_visual/images/back_arrow.svg");
        gtk_button_set_child(GTK_BUTTON(prev_button), prev_icon);
        gtk_widget_add_css_class(prev_button, "nav-button");

        // Window title
        current_window_label = gtk_label_new("Create New User");
        gtk_widget_add_css_class(current_window_label, "window-title");
        gtk_widget_set_size_request(current_window_label, 200, -1); // Fixed size for title
        gtk_label_set_xalign(GTK_LABEL(current_window_label), 0.5);

        // Next button
        GtkWidget *next_button = gtk_button_new();
        gtk_widget_set_size_request(next_button, 40, 40);
        gtk_widget_set_margin_start(next_button, 8);
        GtkWidget *next_icon = gtk_image_new_from_file("src/chat_visual/images/forward_arrow.svg");
        gtk_button_set_child(GTK_BUTTON(next_button), next_icon);
        gtk_widget_add_css_class(next_button, "nav-button");

        // Add navigation elements to right box
        gtk_box_append(GTK_BOX(header_right_box), prev_button);
        gtk_box_append(GTK_BOX(header_right_box), current_window_label);
        gtk_box_append(GTK_BOX(header_right_box), next_button);

        // Connect navigation button signals
        g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_button_clicked), carousel_stack);
        g_signal_connect(prev_button, "clicked", G_CALLBACK(on_prev_button_clicked), carousel_stack);

        // Add right box to grid
        gtk_grid_attach(GTK_GRID(header_box), header_right_box, 1, 0, 1, 1);
    } else {
        g_print("User is NOT admin. Skipping header_right_box.\n");
    }

    // Use alignment for fixed spacing
    gtk_widget_set_hexpand(header_left_box, TRUE); // Left box will take the remaining space
    gtk_widget_set_hexpand(header_box, TRUE);    // Grid won't grow unnecessarily

    // Add header to main box
    gtk_box_append(GTK_BOX(main_box), header_box);


    // Left Box (Nickname and Password Sections)
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_hexpand(left_box, TRUE);
    gtk_widget_set_size_request(left_box, 300, -1);

    // Nickname Section
    GtkWidget *nickname_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(nickname_box, 300, -1);
    GtkWidget *nickname_label = gtk_label_new("Enter new nickname:");
    gtk_widget_set_halign(nickname_label, GTK_ALIGN_START);

    GtkWidget *nickname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(nickname_entry), "Enter nickname");
    gtk_widget_add_css_class(nickname_entry, "nickname-entry");
    gtk_widget_set_size_request(nickname_entry, 250, 20);
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

    GtkWidget *nickname_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);;
    gtk_widget_set_size_request(nickname_separator, 300, 3);
    gtk_widget_add_css_class(nickname_separator, "nickname-separator");
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

    GtkWidget *right_box = NULL;
    
    if (GTK_data->is_admin) {
    // Right Box (Carousel Content)
    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_hexpand(right_box, TRUE);
    gtk_widget_set_size_request(right_box, 300, -1);

    // Create "Create" Window
    GtkWidget *create_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Login input
    GtkWidget *login_label = gtk_label_new("Login:");
    gtk_widget_set_halign(login_label, GTK_ALIGN_START);
    GtkWidget *login_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_entry), "Enter login");
    gtk_widget_add_css_class(login_entry, "login-entry1");

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

    // Create the Admin Change Password section
    GtkWidget *admin_password_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Login input field
    GtkWidget *login_label1 = gtk_label_new("Enter login:");
    gtk_widget_set_halign(login_label1, GTK_ALIGN_START);
    GtkWidget *admin_login_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(admin_login_entry), "Login");
    gtk_widget_add_css_class(admin_login_entry, "login-entry2");
    GTK_data->profile_data->admin_login_entry = admin_login_entry;

    // New password input field
    GtkWidget *password_label1 = gtk_label_new("Enter new password:");
    gtk_widget_set_halign(password_label1, GTK_ALIGN_START);
    GtkWidget *admin_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(admin_password_entry), "New password");
    gtk_entry_set_visibility(GTK_ENTRY(admin_password_entry), FALSE);
    gtk_widget_add_css_class(admin_password_entry, "password-entry");
    GTK_data->profile_data->admin_password_entry = admin_password_entry;

    // Eye icon for the password entry
    GtkWidget *admin_eye_button = gtk_toggle_button_new();
    gtk_button_set_icon_name(GTK_BUTTON(admin_eye_button), "view-conceal-symbolic");
    gtk_widget_set_tooltip_text(admin_eye_button, "Show/Hide Password");
    g_signal_connect(admin_eye_button, "toggled", G_CALLBACK(on_eye_button_clicked), admin_password_entry);
    gtk_widget_add_css_class(admin_eye_button, "eye-icon");

    // Password entry box with overlay
    GtkWidget *admin_password_entry_box = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(admin_password_entry_box), admin_password_entry);
    gtk_overlay_add_overlay(GTK_OVERLAY(admin_password_entry_box), admin_eye_button);

    // Fix alignment and positioning of the eye button
    gtk_widget_set_halign(admin_eye_button, GTK_ALIGN_END); // Align to the end of the entry field
    gtk_widget_set_valign(admin_eye_button, GTK_ALIGN_CENTER); // Center vertically
    gtk_widget_set_margin_end(admin_eye_button, 5); // Add some spacing from the edge

    // Change Password button
    GtkWidget *admin_change_button = gtk_button_new_with_label("Change User Password");
    gtk_widget_add_css_class(admin_change_button, "change-password-button");
    g_signal_connect(admin_change_button, "clicked", G_CALLBACK(on_admin_change_password_button_clicked), GTK_data);

    // Admin Login List Background Container
    GtkWidget *admin_login_list_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Scrollable Admin Login List Container
    GtkWidget *admin_scroll_container = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(admin_scroll_container), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(admin_scroll_container, -1, 200);
    gtk_widget_add_css_class(admin_scroll_container, "admin-login-list-container");

    // Admin Login List
    GtkWidget *admin_login_list = gtk_list_box_new();
    gtk_widget_set_visible(admin_login_list, TRUE); // Ensure visibility
    gtk_widget_set_vexpand(admin_login_list, TRUE); // Allow vertical expansion
    gtk_widget_set_hexpand(admin_login_list, TRUE); // Allow horizontal expansion
    gtk_widget_add_css_class(admin_login_list, "admin-login-list");

    // Set the admin login list as a child of the scroll container
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(admin_scroll_container), admin_login_list);
    GTK_data->profile_data->admin_login_list = GTK_LIST_BOX(admin_login_list);

    // Append the scroll container to the admin login list background
    gtk_box_append(GTK_BOX(admin_login_list_background), admin_scroll_container);

    // Error and success labels
    GtkWidget *admin_error_label = gtk_label_new("");
    gtk_widget_add_css_class(admin_error_label, "error-label");
    GTK_data->profile_data->admin_error_label = admin_error_label;

    GtkWidget *admin_success_label = gtk_label_new("");
    gtk_widget_add_css_class(admin_success_label, "success-label");
    GTK_data->profile_data->admin_success_label = admin_success_label;

    GtkEventController *key_controller3 = gtk_event_controller_key_new();
    g_object_set_data(G_OBJECT(key_controller3), "list-box", admin_login_list);
    g_signal_connect(key_controller3, "key-pressed", G_CALLBACK(on_key_pressed), admin_login_entry);
    gtk_widget_add_controller(admin_login_entry, key_controller3);
    
    // Assemble the UI for admin_password_box
    gtk_box_append(GTK_BOX(admin_password_box), login_label1);
    gtk_box_append(GTK_BOX(admin_password_box), admin_login_entry);
    gtk_box_append(GTK_BOX(admin_password_box), password_label1);
    gtk_box_append(GTK_BOX(admin_password_box), admin_password_entry_box);
    gtk_box_append(GTK_BOX(admin_password_box), admin_change_button);
    gtk_box_append(GTK_BOX(admin_password_box), admin_error_label);
    gtk_box_append(GTK_BOX(admin_password_box), admin_success_label);

    // Append the scrollable admin-login-list at the end
    gtk_box_append(GTK_BOX(admin_password_box), admin_login_list_background);

    g_signal_connect(admin_login_list, "row-selected", G_CALLBACK(on_login_row_selected), admin_login_entry);

    // Add to carousel stack
    gtk_stack_add_titled(GTK_STACK(carousel_stack), admin_password_box, "change_password", "Change Password");

    // Create "Delete" Window
    GtkWidget *delete_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Input field and Delete button
    GtkWidget *delete_label = gtk_label_new("Enter login to deactivate:");
    gtk_widget_set_halign(delete_label, GTK_ALIGN_START);

    GtkWidget *delete_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(delete_entry), "Enter login");
    gtk_widget_add_css_class(delete_entry, "delete-entry");
    GTK_data->profile_data->delete_entry = delete_entry;

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

    // Append scroll container to the background container
    gtk_box_append(GTK_BOX(login_list_background), scroll_container);

    // Add login list background to the delete box
    gtk_box_append(GTK_BOX(delete_box), login_list_background);

    // Add error and success labels here
    GtkWidget *delete_error_label = gtk_label_new("");
    gtk_widget_add_css_class(delete_error_label, "error-label");
    gtk_widget_set_halign(delete_error_label, GTK_ALIGN_START);
    GTK_data->profile_data->error_label = delete_error_label;

    GtkWidget *delete_success_label = gtk_label_new("");
    gtk_widget_add_css_class(delete_success_label, "success-label");
    gtk_widget_set_halign(delete_success_label, GTK_ALIGN_START);
    GTK_data->profile_data->success_label = delete_success_label;

    // Add the labels to the delete_box
    gtk_box_append(GTK_BOX(delete_box), delete_error_label);
    gtk_box_append(GTK_BOX(delete_box), delete_success_label);

    // Add the Delete box to the carousel
    gtk_stack_add_titled(GTK_STACK(carousel_stack), delete_box, "delete", "Delete");

    GtkWidget *activate_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    // Input field and Activate button
    GtkWidget *activate_label = gtk_label_new("Enter login to activate:");
    gtk_widget_set_halign(activate_label, GTK_ALIGN_START);

    GtkWidget *activate_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(activate_entry), "Enter login");
    gtk_widget_add_css_class(activate_entry, "activate-entry");
    GTK_data->profile_data->activate_entry = activate_entry;

    GtkWidget *activate_button = gtk_button_new_with_label("Activation");
    gtk_widget_add_css_class(activate_button, "activate-button");

    GtkWidget *activate_input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    // Ensure widgets have no parent before appending
    if (gtk_widget_get_parent(activate_entry)) {
        gtk_widget_unparent(activate_entry);
    }
    if (gtk_widget_get_parent(activate_button)) {
        gtk_widget_unparent(activate_button);
    }

    // Append widgets to the input box
    gtk_box_append(GTK_BOX(activate_input_box), activate_entry);
    gtk_box_append(GTK_BOX(activate_input_box), activate_button);

    // Add input box to the activate box
    gtk_box_append(GTK_BOX(activate_box), activate_label);
    gtk_box_append(GTK_BOX(activate_box), activate_input_box);

    // Add scrollable list for deactivated users
    GtkWidget *scroll_container2 = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_container2), 
                                GTK_POLICY_NEVER, 
                                GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll_container2, -1, 200);
    gtk_widget_add_css_class(scroll_container2, "login-list-background");

    GtkWidget *deactivated_list = gtk_list_box_new();
    gtk_widget_set_visible(deactivated_list, TRUE);
    gtk_widget_set_vexpand(deactivated_list, TRUE);
    gtk_widget_set_hexpand(deactivated_list, TRUE);
    gtk_widget_add_css_class(deactivated_list, "deactivate-list");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_container2), deactivated_list);
    GTK_data->profile_data->deactivated_list = GTK_LIST_BOX(deactivated_list);

    GtkEventController *key_controller2 = gtk_event_controller_key_new();
    g_object_set_data(G_OBJECT(key_controller2), "list-box", deactivated_list);
    g_signal_connect(key_controller2, "key-pressed", G_CALLBACK(on_key_pressed), activate_entry);
    gtk_widget_add_controller(activate_entry, key_controller2);

    // Connect signals
    g_signal_connect(deactivated_list, "row-selected", G_CALLBACK(on_login_row_selected), activate_entry);

    // Add all widgets to activate_box
    gtk_box_append(GTK_BOX(activate_box), activate_label);
    gtk_box_append(GTK_BOX(activate_box), activate_entry);
    gtk_box_append(GTK_BOX(activate_box), activate_button);
    gtk_box_append(GTK_BOX(activate_box), scroll_container2);

    // Add error and success labels here
    GtkWidget *deactivate_error_label = gtk_label_new("");
    gtk_widget_add_css_class(deactivate_error_label, "error-label");
    gtk_widget_set_halign(deactivate_error_label, GTK_ALIGN_START);
    GTK_data->profile_data->deactivate_error_label = deactivate_error_label;

    GtkWidget *deactivate_success_label = gtk_label_new("");
    gtk_widget_add_css_class(deactivate_success_label, "success-label");
    gtk_widget_set_halign(deactivate_success_label, GTK_ALIGN_START);
    GTK_data->profile_data->deactivate_success_label = deactivate_success_label;

    // Add the labels to the delete_box
    gtk_box_append(GTK_BOX(activate_box), deactivate_error_label);
    gtk_box_append(GTK_BOX(activate_box), deactivate_success_label);

    gtk_stack_add_titled(GTK_STACK(carousel_stack), activate_box, "activate", "Activate User");

    // After creating carousel_stack
    gtk_widget_set_hexpand(carousel_stack, TRUE);
    gtk_widget_set_vexpand(carousel_stack, TRUE);
    gtk_widget_set_visible(carousel_stack, TRUE);

    // When adding to right_box
    gtk_box_append(GTK_BOX(right_box), carousel_stack);
    gtk_widget_set_hexpand(right_box, TRUE);
    gtk_widget_set_vexpand(right_box, TRUE);

    // Prepare data for the callbacks
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 10);
    entries[0] = login_entry;
    entries[1] = password_entry;
    entries[2] = GTK_data->profile_data->error_label;
    entries[3] = user_data;
    entries[4] = window;
    entries[5] = GTK_data->profile_data->success_label;
    entries[6] = admin_check;
    entries[7] = delete_entry;
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_button_clicked), entries);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_deactivate_button_clicked), GTK_data);
    g_signal_connect(activate_button, "clicked", G_CALLBACK(on_activate_button_clicked), GTK_data);

    // Error label for displaying login/password issues
    GTK_data->profile_data->error_label = gtk_label_new("");
    gtk_widget_add_css_class(GTK_data->profile_data->error_label, "error-label");
    gtk_widget_set_halign(GTK_data->profile_data->error_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(create_box), GTK_data->profile_data->error_label);

    // Success label for displaying successful account creation
    GTK_data->profile_data->success_label = gtk_label_new("");
    gtk_widget_add_css_class(GTK_data->profile_data->success_label, "success-label");
    gtk_widget_set_halign(GTK_data->profile_data->success_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(create_box), GTK_data->profile_data->success_label);

    } else {
    g_print("User is NOT admin. Skipping header_right_box.\n");
    }

    // Content Box (Horizontal Split)
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_widget_set_hexpand(content_box, TRUE);
    gtk_widget_set_vexpand(content_box, TRUE);
    gtk_box_append(GTK_BOX(content_box), left_box);
    gtk_box_append(GTK_BOX(content_box), right_box);
    
    // Add header and content box to the main box
    gtk_box_append(GTK_BOX(main_box), header_box);
    gtk_box_append(GTK_BOX(main_box), content_box);
    if (GTK_data->is_admin) {
    cJSON *command = cJSON_CreateObject();
    cJSON_AddNumberToObject(command, "command_code", 17);
    send_and_delete_json(ssl, &command);
    } else {
    g_print("User is NOT admin. Skipping header_right_box.\n");
    }

    gtk_window_set_child(GTK_WINDOW(window), main_box);
    gtk_window_present(GTK_WINDOW(window));
}

void profile_start(GTK_data_t *GTK_data) {
    GtkApplication *app = gtk_application_new("com.example.ProfileForm", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), GTK_data);
    g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
}
