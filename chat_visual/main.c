#include <gtk/gtk.h>
#include <unistd.h>


GtkWidget *messages_container; // Define globally at the top of the file
GtkWidget *message_entry; // Add this global variable for the entry widget
GtkWidget *scrolled_window; // Add this global variable

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

// Callback function for idle scrolling
static gboolean scroll_idle_callback(gpointer data) {
    GtkAdjustment *adj = GTK_ADJUSTMENT(data);
    // Set the value to the maximum to scroll to bottom
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - 
                                gtk_adjustment_get_page_size(adj));
    return G_SOURCE_REMOVE;
}

// Scroll to bottom function
static void scroll_to_bottom(GtkWidget *container) {
    // Get the parent scrolled window of the messages container
    GtkWidget *scrolled = GTK_WIDGET(gtk_widget_get_ancestor(container, GTK_TYPE_SCROLLED_WINDOW));
    if (scrolled) {
        // Get the vertical adjustment
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled));
        // Schedule the scroll after the widget has been allocated and rendered
        g_idle_add(scroll_idle_callback, adj);
    }
}

// Convert milliseconds to seconds
void sleep_ms(int milliseconds)
{
    usleep(milliseconds * 1000);
}

// Function to add a message, appearing at the bottom
void add_message(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent) {
    // Create a horizontal box to contain message and time
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    GtkWidget *message_label = gtk_label_new(message_text);
    gtk_widget_add_css_class(message_label, "message-bubble"); 
    // gtk_widget_add_css_class(message_label, is_sent ? "sent-message-bubble" : "received-message-bubble");   

    // Enable text wrapping
    gtk_label_set_wrap(GTK_LABEL(message_label), TRUE);
    // gtk_widget_set_size_request(message_label, 200, -1);  // Limit width to 200px, height auto-adjusts  

    GtkWidget *time_label = gtk_label_new(time_text);  
    gtk_widget_add_css_class(time_label, "message-time");  

    // Create a new box for alignment  
    GtkWidget *alignment_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); 
    gtk_box_append(GTK_BOX(alignment_box), message_box);   


    if (is_sent) {
        gtk_box_append(GTK_BOX(message_box), time_label);
        gtk_box_append(GTK_BOX(message_box), message_label);
        gtk_widget_set_halign(alignment_box, GTK_ALIGN_END); // Set the alignment to the end if the message is sent
    } else {
        gtk_box_append(GTK_BOX(message_box), message_label);
        gtk_box_append(GTK_BOX(message_box), time_label);
        gtk_widget_set_halign(alignment_box, GTK_ALIGN_START); // Set the alignment to the start if the message is sent
    }

    // Prepend message_box to the top of the container
    gtk_box_append(GTK_BOX(messages_container), alignment_box);

    // Sleep for 500 milliseconds = 0.5 seconds
    sleep_ms(500);
    // Scroll to the bottom after adding the message
    scroll_to_bottom(messages_container);
}

// Main setup for the message container
GtkWidget* create_message_container() {
    scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, -1, 500); // Set maximum height to 500px

    messages_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), messages_container);

    return scrolled_window;
}

GtkWidget* create_chat_item(const char *name, const char *message, const char *time, gboolean is_online, gboolean is_group) {
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_add_css_class(grid, "chat-item");
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);

    // Avatar
    GtkWidget *avatar_circle = gtk_drawing_area_new();
    gtk_widget_set_size_request(avatar_circle, 60, 60);
    gtk_widget_add_css_class(avatar_circle, "avatar-circle");
    gtk_grid_attach(GTK_GRID(grid), avatar_circle, 0, 0, 1, 2);

    // Horizontal box to hold name label and status indicator
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // Name label
    GtkWidget *name_label = gtk_label_new(name);
    gtk_widget_add_css_class(name_label, "chat-name");
    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(name_box), name_label);

    // Status indicator
    if (!is_group) { // Only show status indicator if it's not a group
        GtkWidget *status_indicator = gtk_drawing_area_new();
        gtk_widget_set_size_request(status_indicator, 8, 8);
        gtk_widget_add_css_class(status_indicator, is_online ? "status-online" : "status-offline");
        gtk_box_append(GTK_BOX(name_box), status_indicator);
    }

    // Place name_box at (1, 0)
    gtk_grid_attach(GTK_GRID(grid), name_box, 1, 0, 1, 1);

    // Message preview label
    GtkWidget *message_label = gtk_label_new(message);
    gtk_widget_add_css_class(message_label, "message-preview");
    gtk_widget_set_halign(message_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), message_label, 1, 1, 1, 1);

    // Time label
    GtkWidget *time_label = gtk_label_new(time);
    gtk_widget_add_css_class(time_label, "message-time");
    gtk_widget_set_halign(time_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), time_label, 2, 0, 1, 2);

    if (is_group) {
        gtk_widget_add_css_class(grid, "group-chat"); // Apply a specific style for group chats
    }

    return grid;
}

// Function to create the contact sidebar
GtkWidget* create_contact_sidebar() {
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    GtkWidget *contact_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_add_css_class(contact_box, "contact-box");
    gtk_widget_set_valign(contact_box, GTK_ALIGN_START); // Align contacts to the top
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), contact_box);

    // Add chat items
    GtkWidget *chat1 = create_chat_item("Vladyslav Zaplitnyi", "I love two things, one is...", "12:00", TRUE, FALSE);
    GtkWidget *chat2 = create_chat_item("Vladyslav Nezapitnyi", "I love two things, one is...", "11:40", FALSE, FALSE);
    GtkWidget *chat3 = create_chat_item("Group chat", "I love two things, one is...", "11:00", FALSE, TRUE);

    gtk_box_append(GTK_BOX(contact_box), chat1);
    gtk_box_append(GTK_BOX(contact_box), chat2);
    gtk_box_append(GTK_BOX(contact_box), chat3);

    return scrolled_window;
}

// New callback function for send button
static void on_send_clicked(GtkWidget *button, gpointer user_data) {
    const char *message_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(message_entry))
    );
    
    // Only send if the message is not empty
    if (message_text && strlen(message_text) > 0) {
        // Get current time
        GDateTime *now = g_date_time_new_now_local();
        char *time_str = g_date_time_format(now, "%H:%M");
        
        // Add the message
        add_message(messages_container, message_text, time_str, TRUE);
        
        // Clear the entry
        gtk_entry_buffer_set_text(
            gtk_entry_get_buffer(GTK_ENTRY(message_entry)),
            "",
            0
        );
        
        // Free resources
        g_free(time_str);
        g_date_time_unref(now);
    }
}

// Add this function after on_send_clicked, to send the message when you press enter
static void on_entry_activated(GtkEntry *entry, gpointer user_data) {
    // Simulate send button click when Enter is pressed
    on_send_clicked(NULL, NULL);
}

// Main application window setup
static void on_activate(GtkApplication *app) {
    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 1800, 800); //1800, 1000 було

    // Apply CSS to window
    apply_css(window);

    // Create the main grid for layout
    GtkWidget *main_grid = gtk_grid_new();
    gtk_widget_set_margin_top(main_grid, 20);
    gtk_widget_set_margin_bottom(main_grid, 20);
    gtk_widget_set_margin_start(main_grid, 20);
    gtk_widget_set_margin_end(main_grid, 20);
    gtk_widget_set_hexpand(main_grid, TRUE); // Make the main grid horizontally expandable
    gtk_widget_set_vexpand(main_grid, TRUE); // Make the main grid vertically expandable

    // Set a fixed width for the main grid
    // gtk_widget_set_size_request(main_grid, 1800, -1);

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

    // chats and groups
    GtkWidget *chat1 = create_chat_item("Vladyslav Zaplitnyi", "I love two things, one is...", "12:00", TRUE, FALSE);
    gtk_box_append(GTK_BOX(sidebar), chat1);

    GtkWidget *chat2 = create_chat_item("Vladyslav Nezapitnyi", "I love two things, one is...", "11:40", FALSE, FALSE);
    gtk_box_append(GTK_BOX(sidebar), chat2);

    GtkWidget *chat3 = create_chat_item("Group chat", "I love two things, one is...", "11:00", TRUE, TRUE);
    gtk_widget_add_css_class(chat3, "group-chat"); // Add specific class for group chat if needed
    gtk_box_append(GTK_BOX(sidebar), chat3);


    // add new group button
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
    GtkWidget *messages_container_wrapper = create_message_container();
    gtk_widget_add_css_class(messages_container_wrapper, "messages-container");
    gtk_box_append(GTK_BOX(chat_area_background), messages_container_wrapper);

    // Sample messages
    add_message(messages_container, "Hello!", "10:00", FALSE);
    add_message(messages_container, "How are you?", "10:05", TRUE);
    add_message(messages_container, "I'm doing well, thanks!", "10:07", FALSE);
    add_message(messages_container, "Love you!!! <3", "12:00", TRUE);
    add_message(messages_container, "Love you!!! <3 You mean everything for me dude!", "12:00", FALSE);


    // --- Message Input Area ---
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_top(input_box, 10);
    gtk_widget_add_css_class(input_box, "input-box");

    message_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_entry), "Enter message");
    gtk_widget_set_hexpand(message_entry, TRUE);
    gtk_widget_add_css_class(message_entry, "message-entry");
    gtk_box_append(GTK_BOX(input_box), message_entry);

    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_widget_add_css_class(send_button, "send-button");
    // Connect the callback to the send button
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_clicked), NULL);
    // In the on_activate function, after creating message_entry, add:
    g_signal_connect(message_entry, "activate", G_CALLBACK(on_entry_activated), NULL);
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