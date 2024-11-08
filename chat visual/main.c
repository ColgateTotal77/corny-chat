#include <gtk/gtk.h>

// Function to apply CSS styles
static void apply_css(GtkWidget *widget) {
    GtkCssProvider *css_provider = gtk_css_provider_new();

    // Load CSS with only two parameters
    gtk_css_provider_load_from_path(css_provider, "style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(css_provider);
}

// Scroll to the bottom of the chat area
void scroll_to_bottom(GtkWidget *scrolled_window) {
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));
}

// Function to add a message with time
void add_message(GtkWidget *container, const char *message_text, const char *time_text, gboolean is_sent) {
    // Create a horizontal box for aligning message and time side-by-side
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(message_box, is_sent ? GTK_ALIGN_END : GTK_ALIGN_START);

    // Time and message labels
    GtkWidget *time_label = gtk_label_new(time_text);
    gtk_widget_add_css_class(time_label, "message-time");

    GtkWidget *message_label = gtk_label_new(message_text);
    gtk_widget_add_css_class(message_label, is_sent ? "sent-message-bubble" : "received-message-bubble");

    // Add time and message to the message box based on message type
    if (is_sent) {
        gtk_box_append(GTK_BOX(message_box), time_label);
        gtk_box_append(GTK_BOX(message_box), message_label);
    } else {
        gtk_box_append(GTK_BOX(message_box), message_label);
        gtk_box_append(GTK_BOX(message_box), time_label);
    }

    // Add the message_box to the main container
    gtk_box_append(GTK_BOX(container), message_box);
}

// Main application window setup
static void on_activate(GtkApplication *app) {
    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 1800, 1000);

    // Apply CSS to window
    apply_css(window);

    // Create the main grid for layout
    GtkWidget *main_grid = gtk_grid_new();
    gtk_widget_set_margin_top(main_grid, 20);
    gtk_widget_set_margin_bottom(main_grid, 20);
    gtk_widget_set_margin_start(main_grid, 20);
    gtk_widget_set_margin_end(main_grid, 20);

    // --- Top Panel (Settings and Search Bar) ---
    GtkWidget *top_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_grid_attach(GTK_GRID(main_grid), top_panel, 0, 0, 1, 1);

    // Settings button setup
    GtkWidget *settings_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(settings_background, "settings-background");
    gtk_box_append(GTK_BOX(top_panel), settings_background);

    GtkWidget *settings_button = gtk_button_new();
    gtk_widget_add_css_class(settings_button, "settings-button");
    gtk_box_append(GTK_BOX(settings_background), settings_button);

    GtkWidget *settings_icon = gtk_image_new_from_file("images/settings.svg");
    gtk_button_set_child(GTK_BUTTON(settings_button), settings_icon);

    // Search bar setup
    GtkWidget *search_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(search_container, "search-container");
    gtk_box_append(GTK_BOX(top_panel), search_container);

    GtkWidget *search_icon = gtk_image_new_from_file("images/search.svg");
    gtk_widget_set_size_request(search_icon, 28, 28);
    gtk_box_append(GTK_BOX(search_container), search_icon);

    GtkWidget *search_bar = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_bar), "Search");
    gtk_widget_add_css_class(search_bar, "search-bar");
    gtk_box_append(GTK_BOX(search_container), search_bar);

    // --- Sidebar Setup ---
    GtkWidget *sidebar_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(sidebar_background, "sidebar-background");
    gtk_grid_attach(GTK_GRID(main_grid), sidebar_background, 0, 1, 1, 2);

    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(sidebar, 350, 400);
    gtk_widget_add_css_class(sidebar, "sidebar");
    gtk_box_append(GTK_BOX(sidebar_background), sidebar);

    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(sidebar), spacer);

    GtkWidget *add_group_button = gtk_button_new_with_label("Add new group");
    gtk_widget_add_css_class(add_group_button, "add-group-button");
    gtk_box_append(GTK_BOX(sidebar), add_group_button);

    // --- Chat Area Setup ---
    GtkWidget *chat_area_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(chat_area_background, "chat-area-background");
    gtk_grid_attach(GTK_GRID(main_grid), chat_area_background, 1, 1, 1, 1);

    // Chat header
    GtkWidget *chat_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_size_request(chat_header, -1, 60);
    gtk_widget_add_css_class(chat_header, "chat-header");

    GtkWidget *avatar_circle = gtk_drawing_area_new();
    gtk_widget_set_size_request(avatar_circle, 60, 60);
    gtk_widget_add_css_class(avatar_circle, "avatar-circle");
    gtk_box_append(GTK_BOX(chat_header), avatar_circle);

    GtkWidget *chat_user_label = gtk_label_new("Vladyslav Zaplitnyi");
    gtk_widget_add_css_class(chat_user_label, "header-name");
    gtk_box_append(GTK_BOX(chat_header), chat_user_label);

    GtkWidget *status_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(status_container, "status-container");

    GtkWidget *status_indicator = gtk_drawing_area_new();
    gtk_widget_set_size_request(status_indicator, 10, 10);
    gtk_widget_add_css_class(status_indicator, "status-indicator");
    gtk_box_append(GTK_BOX(status_container), status_indicator);

    GtkWidget *status_text = gtk_label_new("online");
    gtk_widget_add_css_class(status_text, "status-text");
    gtk_box_append(GTK_BOX(status_container), status_text);
    gtk_box_append(GTK_BOX(chat_header), status_container);

    // Chat messages container
    GtkWidget *messages_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_vexpand(messages_container, TRUE);
    gtk_widget_add_css_class(messages_container, "messages-container");
    gtk_box_append(GTK_BOX(chat_area_background), messages_container);

    // Sample messages
    add_message(messages_container, "Love you!!! <3 You mean everything for me dude!", "12:00", FALSE);
    add_message(messages_container, "Love you!!! <3", "12:00", TRUE);

    // --- Message Input Area ---
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_top(input_box, 10);
    gtk_widget_add_css_class(input_box, "input-box");

    GtkWidget *message_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_entry), "Enter message");
    gtk_widget_set_hexpand(message_entry, TRUE);
    gtk_widget_add_css_class(message_entry, "message-entry");
    gtk_box_append(GTK_BOX(input_box), message_entry);

    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_widget_add_css_class(send_button, "send-button");
    gtk_box_append(GTK_BOX(input_box), send_button);

    // Layout setup
    gtk_grid_attach(GTK_GRID(main_grid), chat_header, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), input_box, 1, 2, 1, 1);

    // Add main grid to window
    gtk_window_set_child(GTK_WINDOW(window), main_grid);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.ChatApp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
