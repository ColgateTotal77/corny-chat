#include "GTK.h"

static void on_entry_activated(GtkEntry *entry, gpointer user_data) {
    (void) entry;
    on_send_clicked(NULL, user_data);
}

static void on_settings_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    if (!GTK_data) return;
    
    // Get the current window
    GtkWidget *current_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
    if (!current_window) return;
    
    // Hide the current window
    //gtk_widget_set_visible(current_window, true);


    // Start profile form
    profile_start(GTK_data);
    
    //apply_css("src/chat_visual/gtk_src/GTK_Start/style.css");
    // Show the main window again
    //gtk_window_present(GTK_WINDOW(current_window));
}

void show_all_contacts(GTK_data_t *GTK_data){
    if (!GTK_data || !GTK_data->chat_manager) {
        printf("Chat manager is NULL.\n");
        return;
    }

    chat_manager_t *chat_manager = GTK_data->chat_manager;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, chat_manager->chats);
    
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        chat_data_t *chat_data = (chat_data_t *)value;
        if(!chat_data->is_show){
            chat_data->is_show = true;
            gtk_widget_set_visible(chat_data->button, TRUE);
        }
    }
}

// Function to print chat manager information
void print_chat_manager_info(GTK_data_t *GTK_data, const char *search_enter) {
    if (!GTK_data || !GTK_data->chat_manager) {
        printf("Chat manager is NULL.\n");
        return;
    }

    // Print all chats in chat_manager
    chat_manager_t *chat_manager = GTK_data->chat_manager;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, chat_manager->chats);

    int search_length = strlen(search_enter);
    if(search_length > 0){
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            chat_data_t *chat_data = (chat_data_t *)value;

            if(search_enter && mx_strstr(chat_data->contact_name, search_enter) != 0){
                chat_data->is_show = true;
                gtk_widget_set_visible(chat_data->button, TRUE);
            } else {
                chat_data->is_show = false;
                gtk_widget_set_visible(chat_data->button, FALSE);
            }
        }
    }else{
        show_all_contacts(GTK_data);
    }
}

// Signal handler for search bar changes
void on_search_bar_changed(GtkEntry *entry, gpointer user_data) {
    (void)entry;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;
    const char *enter = gtk_editable_get_text(GTK_EDITABLE(entry));
    print_chat_manager_info(GTK_data, enter);
}

void on_erase_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;
    // Clear the search bar text
    if (GTK_data->search_bar) {
        gtk_editable_set_text(GTK_EDITABLE(GTK_data->search_bar), ""); // Clear the search bar
    }
    show_all_contacts(GTK_data);
}


// Main application window setup
static void on_activate(GtkApplication *app, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    
    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 1800, 800); //1800, 1000 було
    GTK_data->window = window;


    // Create the main grid for layout
    GtkWidget *main_grid = gtk_grid_new();
    gtk_widget_set_margin_top(main_grid, 20);
    gtk_widget_set_margin_bottom(main_grid, 20);
    gtk_widget_set_margin_start(main_grid, 20);
    gtk_widget_set_margin_end(main_grid, 20);
    gtk_widget_set_hexpand(main_grid, TRUE); // Make the main grid horizontally expandable
    gtk_widget_set_vexpand(main_grid, TRUE); // Make the main grid vertically expandable

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

    GtkWidget *settings_icon = gtk_image_new_from_file("src/chat_visual/images/settings.svg");
    gtk_button_set_child(GTK_BUTTON(settings_button), settings_icon);

    // Add this line to connect the callback:
    g_signal_connect(settings_button, "clicked", G_CALLBACK(on_settings_clicked), GTK_data);

    // Search bar setup
    GtkWidget *search_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(search_container, "search-container");
    gtk_box_append(GTK_BOX(top_panel), search_container);

    GtkWidget *search_icon = gtk_image_new_from_file("src/chat_visual/images/search.svg");
    gtk_widget_set_size_request(search_icon, 28, 28);
    gtk_box_append(GTK_BOX(search_container), search_icon);

    GTK_data->search_bar = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(GTK_data->search_bar), "Search");
    gtk_widget_add_css_class(GTK_data->search_bar, "search-bar");
    gtk_box_append(GTK_BOX(search_container), GTK_data->search_bar);

    GtkWidget *erase_button = gtk_button_new_with_label("X");
    gtk_widget_add_css_class(erase_button, "search-erase-button");
    gtk_widget_set_size_request(erase_button, 28, 28);
    gtk_box_append(GTK_BOX(search_container), erase_button);

    g_signal_connect(erase_button, "clicked", G_CALLBACK(on_erase_button_clicked), GTK_data);
    g_signal_connect(GTK_data->search_bar, "changed", G_CALLBACK(on_search_bar_changed), GTK_data);

    // --- Sidebar Setup ---
    GtkWidget *sidebar_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(sidebar_background, "sidebar-background");
    gtk_grid_attach(GTK_GRID(main_grid), sidebar_background, 0, 1, 1, 2);

    GtkWidget *sidebar_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sidebar_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(sidebar_scroll, TRUE);
    gtk_box_append(GTK_BOX(sidebar_background), sidebar_scroll);

    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(sidebar, 350, -1);
    gtk_widget_add_css_class(sidebar, "sidebar");
    gtk_widget_set_hexpand(sidebar, FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sidebar_scroll), sidebar);

    // --- Chat Area Setup ---
    GtkWidget *chat_area_background = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(chat_area_background, "chat-area-background");
    gtk_grid_attach(GTK_GRID(main_grid), chat_area_background, 1, 1, 1, 1);

    // Chat header
    GtkWidget *chat_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_size_request(chat_header, -1, 60);
    gtk_widget_add_css_class(chat_header, "chat-header");

    GtkWidget *avatar_circle = gtk_image_new_from_file("src/chat_visual/images/person.svg");
    gtk_widget_set_size_request(avatar_circle, 60, 60);
    //gtk_widget_add_css_class(avatar_circle, "avatar-circle");
    gtk_box_append(GTK_BOX(chat_header), avatar_circle);

    GtkWidget *input_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); 
    gtk_widget_set_margin_top(input_container, 10);

    GtkWidget *error_label = gtk_label_new("Too long message > 500");
    gtk_widget_add_css_class(error_label, "error-label");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_widget_set_visible(error_label, FALSE);
    gtk_box_append(GTK_BOX(input_container), error_label);

    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_add_css_class(input_box, "input-box");
    gtk_box_append(GTK_BOX(input_container), input_box);

    // Create message entry
    GtkWidget *message_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_entry), "Enter message");
    gtk_widget_set_hexpand(message_entry, TRUE);
    gtk_widget_add_css_class(message_entry, "message-entry");
    gtk_box_append(GTK_BOX(input_box), message_entry);

    GtkWidget *cancel_button = gtk_button_new_with_label("X");
    gtk_widget_add_css_class(cancel_button, "search-erase-button");
    gtk_widget_set_size_request(cancel_button, 20, 20);
    gtk_box_append(GTK_BOX(input_box), cancel_button); 
    gtk_widget_set_visible(cancel_button, false);

    chat_manager_t *chat_manager = g_new(chat_manager_t, 1);
    chat_manager->chats = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    chat_manager->active_chat = NULL;
    chat_manager->message_entry = message_entry;
    chat_manager->sidebar = sidebar;
    chat_manager->error_label = error_label;
    chat_manager->is_editing = g_new(gboolean, 1);
    *chat_manager->is_editing = false;
    chat_manager->cancel_button = cancel_button;

    GtkWidget *chat_user_label = gtk_label_new("");
    gtk_widget_add_css_class(chat_user_label, "header-name");
    gtk_box_append(GTK_BOX(chat_header), chat_user_label);
    chat_manager->chat_user_label = chat_user_label;

    GTK_data->chat_manager = chat_manager;
    chat_manager->active_chat = NULL;
    chat_manager->input_box = input_box;
    chat_manager->chat_header = chat_header;
    chat_manager->chat_area_background = chat_area_background;

    // Add new group button
    GtkWidget *add_group_button = gtk_button_new_with_label("Add new group");
    gtk_widget_add_css_class(add_group_button, "add-group-button");
    gtk_box_append(GTK_BOX(sidebar), add_group_button);

    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_widget_add_css_class(send_button, "send-button");
    chat_manager->send_button = send_button;
    // Connect the callback to the send button
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_clicked), GTK_data);    
    // In the on_activate function, after creating message_entry, add:
    g_signal_connect(message_entry, "activate", G_CALLBACK(on_entry_activated), GTK_data);    
    gtk_box_append(GTK_BOX(input_box), send_button);

    gtk_widget_set_visible(input_box, false);
    gtk_widget_set_visible(chat_area_background, false);
    gtk_widget_set_visible(chat_header, false);

    GtkWidget *select_a_chat_label = gtk_label_new("Select a chat to start messaging");
    gtk_widget_add_css_class(select_a_chat_label, "placeholder-message");
    gtk_grid_attach(GTK_GRID(main_grid), select_a_chat_label, 1, 1, 1, 1);
    gtk_widget_set_size_request(select_a_chat_label, -1, 500);
    gtk_widget_set_hexpand(select_a_chat_label, true);
    gtk_widget_set_visible(select_a_chat_label, true);
    chat_manager->select_a_chat_label = select_a_chat_label;
    
    // Layout setup
    gtk_grid_attach(GTK_GRID(main_grid), chat_header, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), input_container, 1, 2, 1, 1);

    // Add main grid to window
    gtk_window_set_child(GTK_WINDOW(window), main_grid);
    gtk_window_present(GTK_WINDOW(window));
    
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, &send_msg_handler, (void*)GTK_data) != 0) {
        printf("ERROR: pthread\n");
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, (void*)GTK_data) != 0) {
        printf("ERROR: pthread\n");
    }

}

void GTK_start(GTK_data_t *GTK_data) {
    static GtkApplication *app = NULL;

    if (app == NULL) {
        GTK_data->profile_data = (profile_data_t *)malloc(sizeof(profile_data_t));
        GTK_data->profile_data->login_list = NULL;
        
        app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_NON_UNIQUE);
        g_signal_connect(app, "activate", G_CALLBACK(on_activate), GTK_data);
    }

    g_application_run(G_APPLICATION(app), 0, NULL);
}
