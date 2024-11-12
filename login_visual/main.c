// Include necessary libraries
#include <gtk/gtk.h>
#include <stdio.h>
#include "cJSON.h" // Ensure you have cJSON library installed and accessible

/* Apply CSS styling to the widget and its children */
static void apply_css(GtkWidget *widget) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GError *error = NULL;
    
    // Create a GFile for the CSS file
    GFile *file = g_file_new_for_path("style.css");
    gtk_css_provider_load_from_file(css_provider, file);
    
    if (error) {
        g_warning("Error loading CSS: %s", error->message);
        g_error_free(error);
    } else {
        gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                                   GTK_STYLE_PROVIDER(css_provider),
                                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
    
    g_object_unref(file);
    g_object_unref(css_provider);
}

/* Function to convert login and password to JSON and print */
static void print_json_data(const char *login, const char *password) {
    cJSON *json = cJSON_CreateObject(); // Create JSON object
    cJSON_AddStringToObject(json, "name", login);
    cJSON_AddStringToObject(json, "password", password);

    // Convert JSON object to a string and print
    char *json_str = cJSON_Print(json);
    printf("%s\n", json_str);

    // Free allocated memory
    cJSON_Delete(json);
    free(json_str);
}

/* Callback function to handle the login button click */
static void on_login_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget **entries = (GtkWidget **)user_data;
    const char *login = gtk_editable_get_text(GTK_EDITABLE(entries[0]));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(entries[1]));

    // Print login and password to the console in JSON format
    print_json_data(login, password);
}

static void on_activate(GtkApplication *app) {
    // Create a new window with specific dimensions
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    
    // Create main container with responsive padding
    GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(main_container, 40);
    gtk_widget_set_margin_end(main_container, 40);
    
    // Create a vertical box with flexible spacing
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_vexpand(vbox, TRUE);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
    
    // Login field container
    GtkWidget *login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *login_label = gtk_label_new("Login:");
    gtk_widget_add_css_class(login_label, "input-label");
    gtk_widget_set_halign(login_label, GTK_ALIGN_START);
    GtkWidget *login_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_entry), "Enter login");
    gtk_widget_set_hexpand(login_entry, TRUE);
    gtk_box_append(GTK_BOX(login_box), login_label);
    gtk_box_append(GTK_BOX(login_box), login_entry);
    
    // Password field container
    GtkWidget *password_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_add_css_class(password_label, "input-label");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_widget_set_hexpand(password_entry, TRUE);
    gtk_box_append(GTK_BOX(password_box), password_label);
    gtk_box_append(GTK_BOX(password_box), password_entry);

    // Remember me checkbox and login button
    GtkWidget *remember_me_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *remember_me_check = gtk_check_button_new_with_label("Remember me");
    gtk_widget_add_css_class(remember_me_check, "remember-me-checkbox");
    gtk_widget_set_hexpand(remember_me_check, TRUE);

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    gtk_widget_add_css_class(login_button, "login-button");

    // Store login and password entries in an array for callback
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 2);
    entries[0] = login_entry;
    entries[1] = password_entry;

    // Connect the login button signal
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), entries);

    gtk_box_append(GTK_BOX(remember_me_box), remember_me_check);
    gtk_box_append(GTK_BOX(remember_me_box), login_button);
    
    // Add all elements to the main vertical box
    gtk_box_append(GTK_BOX(vbox), login_box);
    gtk_box_append(GTK_BOX(vbox), password_box);
    gtk_box_append(GTK_BOX(vbox), remember_me_box);
    
    // Add vbox to main container
    gtk_box_append(GTK_BOX(main_container), vbox);
    
    // Apply CSS and connect signals
    apply_css(window);

    // Set the main container as the window's child and present the window
    gtk_window_set_child(GTK_WINDOW(window), main_container);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    // Create and run the GTK application
    GtkApplication *app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
