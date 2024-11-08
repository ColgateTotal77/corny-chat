// Include gtk
#include <gtk/gtk.h>

// Function to load the external CSS file
static void apply_css(GtkWidget *widget) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "style.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(css_provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);
}

static void on_activate(GtkApplication *app) {
    // Create a new window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);

    // Apply the CSS to the window
    apply_css(window);

    // Create a vertical box layout to stack widgets vertically
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);

    // Create and style the "Username" field
    GtkWidget *username_label = gtk_label_new("Username:");
    gtk_widget_set_halign(username_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(vbox), username_label);

    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter username");
    gtk_widget_set_halign(username_entry, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(username_entry, TRUE);
    gtk_widget_set_size_request(username_entry, 400, -1);
    gtk_box_append(GTK_BOX(vbox), username_entry);

    // Create and style the "Password" field
    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(vbox), password_label);

    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_widget_set_halign(password_entry, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(password_entry, TRUE);
    gtk_widget_set_size_request(password_entry, 400, -1);
    gtk_box_append(GTK_BOX(vbox), password_entry);

    // Create the "Login" button with a special class
    GtkWidget *login_button = gtk_button_new_with_label("Login");
    gtk_widget_set_halign(login_button, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(login_button, TRUE);
    gtk_widget_set_size_request(login_button, 400, -1);
    gtk_widget_add_css_class(login_button, "login-button"); // Add custom CSS class
    gtk_box_append(GTK_BOX(vbox), login_button);

    // Close the window when "Login" button is clicked
    g_signal_connect_swapped(login_button, "clicked", G_CALLBACK(gtk_window_close), window);

    // Set the vertical box as the main child of the window
    gtk_window_set_child(GTK_WINDOW(window), vbox);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.GtkApplication",
                                              G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
