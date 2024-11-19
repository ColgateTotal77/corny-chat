#include "GTK.h"

//GtkWidget *messages_container = NULL; 
// GtkWidget *messages_container; // Define globally at the top of the file
// GtkWidget *message_entry; // Add this global variable for the entry widget
// GtkWidget *scrolled_window; // Add this global variable

// void on_send_clicked_wrapper(gpointer user_data) {
//     GTK_data_t *GTK_data = (GTK_data_t*)user_data;

//     // printf("on_send_clicked_wrapper: Locking message_mutex\n");
//     // pthread_mutex_lock(&GTK_data->message_mutex);

//     char* message = on_send_clicked(user_data);
//     if (message != NULL) {
//         printf("on_send_clicked_wrapper: Message received: %s\n", message);
//         if (GTK_data->message != NULL) {
//             printf("on_send_clicked_wrapper: Freeing old message\n");
//             free(GTK_data->message);
//         }
//         GTK_data->message = mx_strdup(message);
//         printf("on_send_clicked_wrapper: Duplicated message: %s\n", GTK_data->message);
//         free(message);
//     }else{
//         printf("on_send_clicked_wrapper: No message received\n");
//     }

//     // printf("on_send_clicked_wrapper: Unlocking message_mutex\n");
//     // pthread_mutex_unlock(&GTK_data->message_mutex);
// }


// Function to apply CSS styles
static void apply_css(GtkWidget *widget) {
    (void)widget;
    GtkCssProvider *css_provider = gtk_css_provider_new();

    // Load CSS with only two parameters
    gtk_css_provider_load_from_path(css_provider, "src/chat_visual/gtk_src/style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(css_provider);
}

// Main setup for the message container
// GtkWidget* create_message_container(void) {
//     scrolled_window = gtk_scrolled_window_new();
//     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
//     gtk_widget_set_size_request(scrolled_window, -1, 500);

//     GtkWidget *messages_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), messages_container);

//     return scrolled_window;
// }

// GtkWidget* create_chat_item(const char *name, const char *message, const char *time, gboolean is_online, gboolean is_group) {
//     GtkWidget *grid = gtk_grid_new();
//     gtk_widget_add_css_class(grid, "chat-item");
//     gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
//     gtk_grid_set_row_spacing(GTK_GRID(grid), 5);

//     // Avatar
//     GtkWidget *avatar_circle = gtk_drawing_area_new();
//     gtk_widget_set_size_request(avatar_circle, 60, 60);
//     gtk_widget_add_css_class(avatar_circle, "avatar-circle");
//     gtk_grid_attach(GTK_GRID(grid), avatar_circle, 0, 0, 1, 2);

//     // Horizontal box to hold name label and status indicator
//     GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

//     // Name label
//     GtkWidget *name_label = gtk_label_new(name);
//     gtk_widget_add_css_class(name_label, "chat-name");
//     gtk_widget_set_halign(name_label, GTK_ALIGN_START);
//     gtk_box_append(GTK_BOX(name_box), name_label);

//     // Status indicator
//     if (!is_group) { // Only show status indicator if it's not a group
//         GtkWidget *status_indicator = gtk_drawing_area_new();
//         gtk_widget_set_size_request(status_indicator, 8, 8);
//         gtk_widget_add_css_class(status_indicator, is_online ? "status-online" : "status-offline");
//         gtk_box_append(GTK_BOX(name_box), status_indicator);
//     }

//     // Place name_box at (1, 0)
//     gtk_grid_attach(GTK_GRID(grid), name_box, 1, 0, 1, 1);

//     // Message preview label
//     GtkWidget *message_label = gtk_label_new(message);
//     gtk_widget_add_css_class(message_label, "message-preview");
//     gtk_widget_set_halign(message_label, GTK_ALIGN_START);
//     gtk_grid_attach(GTK_GRID(grid), message_label, 1, 1, 1, 1);

//     // Time label
//     GtkWidget *time_label = gtk_label_new(time);
//     gtk_widget_add_css_class(time_label, "message-time");
//     gtk_widget_set_halign(time_label, GTK_ALIGN_END);
//     gtk_grid_attach(GTK_GRID(grid), time_label, 2, 0, 1, 2);

//     if (is_group) {
//         gtk_widget_add_css_class(grid, "group-chat"); // Apply a specific style for group chats
//     }

//     return grid;
// }

// Function to create the contact sidebar
GtkWidget* create_contact_sidebar(void) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    GtkWidget *contact_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_add_css_class(contact_box, "contact-box");
    gtk_widget_set_valign(contact_box, GTK_ALIGN_START); // Align contacts to the top
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), contact_box);

    //Add chat items
    // GtkWidget *chat1 = create_chat_item("Vladyslav Zaplitnyi", "I love two things, one is...", "12:00", TRUE, FALSE);
    // GtkWidget *chat2 = create_chat_item("Vladyslav Nezapitnyi", "I love two things, one is...", "11:40", FALSE, FALSE);
    // GtkWidget *chat3 = create_chat_item("Group chat", "I love two things, one is...", "11:00", FALSE, TRUE);

    // gtk_box_append(GTK_BOX(contact_box), chat1);
    // gtk_box_append(GTK_BOX(contact_box), chat2);
    // gtk_box_append(GTK_BOX(contact_box), chat3);

    return scrolled_window;
}

// New callback function for send button
// void on_send_clicked (GtkWidget *messages_container, GtkWidget *message_entry) {
//     const char *message_text = gtk_entry_buffer_get_text(
//         gtk_entry_get_buffer(GTK_ENTRY(message_entry))
//     );
    
//     // Only send if the message is not empty
//     if (message_text && strlen(message_text) > 0) {
//         // Get current time
//         GDateTime *now = g_date_time_new_now_local();
//         char *time_str = g_date_time_format(now, "%H:%M");
        
//         // Add the message
//         add_message(messages_container, message_text, time_str, TRUE);
        
        //// call_data_t *call_data = (call_data_t*)user_data;
        //// send_to_user(call_data->ssl, 3, (char *)message_text);

//         // Clear the entry
//         gtk_entry_buffer_set_text(
//             gtk_entry_get_buffer(GTK_ENTRY(message_entry)),
//             "",
//             0
//         );
        
//         // Free resources
//         g_free(time_str);
//         g_date_time_unref(now);
//     }
// }

// Add this function after on_send_clicked, to send the message when you press enter
static void on_entry_activated(GtkEntry *entry, gpointer user_data) {
    (void) entry;
    on_send_clicked(NULL, user_data);
}

// void* recv_msg_handler_wrapper(void* arg) {
//     call_data_t* call_data = (call_data_t*)arg;
//     recv_msg_handler(call_data, messages_container);  

//     return NULL;
// }

// void* send_msg_handler_wrapper(void* arg) {
//     GTK_data_t* GTK_data = (GTK_data_t*)arg;
//     send_msg_handler(GTK_data);  

//     return NULL;
// }

// Main application window setup
static void on_activate(GtkApplication *app, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    
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

    GtkWidget *settings_icon = gtk_image_new_from_file("src/chat_visual/images/settings.svg");
    gtk_button_set_child(GTK_BUTTON(settings_button), settings_icon);

    // Search bar setup
    GtkWidget *search_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(search_container, "search-container");
    gtk_box_append(GTK_BOX(top_panel), search_container);

    GtkWidget *search_icon = gtk_image_new_from_file("src/chat_visual/images/search.svg");
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

    GtkWidget *sidebar_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sidebar_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(sidebar_scroll, TRUE);
    gtk_box_append(GTK_BOX(sidebar_background), sidebar_scroll);
    
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(sidebar, 350, -1);
    gtk_widget_add_css_class(sidebar, "sidebar");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sidebar_scroll), sidebar);

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

    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_top(input_box, 10);
    gtk_widget_add_css_class(input_box, "input-box");

    // Create message entry
    GtkWidget *message_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_entry), "Enter message");
    gtk_widget_set_hexpand(message_entry, TRUE);
    gtk_widget_add_css_class(message_entry, "message-entry");
    gtk_box_append(GTK_BOX(input_box), message_entry);
    GTK_data->message_entry = message_entry;
    
    // Создаем менеджер чатов
    chat_manager_t *chat_manager = g_new(chat_manager_t, 1);
    chat_manager->chats = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    chat_manager->active_chat = NULL;
    chat_manager->chat_user_label = chat_user_label;
    chat_manager->chat_area_background = chat_area_background;
    
    GTK_data->user_list = create_user_list();
    
    create_user(GTK_data->user_list, "John Doe", 1, true, "Hello!", "12:30");
    create_user(GTK_data->user_list, "Jane Smith", 2, false, "See you later", "11:45");
    create_user(GTK_data->user_list, "Team Alpha", 3, true, "Meeting at 2 PM", "10:15");
    create_user(GTK_data->user_list, "Bob Wilson", 4, true, "Thanks!", "09:30");
    create_user(GTK_data->user_list, "Project Beta", 5, true, "Updates ready", "08:45");

    //Створюємо перший чат за замовчуванням
    char *contacts[] = {
        "Notes"
    };
    
    int contact_id[] = {0};

    chat_data_t *chat_data = create_chat_data(contacts[0], contact_id[0]);
    g_hash_table_insert(chat_manager->chats, GINT_TO_POINTER(contact_id[0]), chat_data);

    GtkWidget *chat_item = create_chat_item(contacts[0], contact_id[0], "None", "12:00", true, false, chat_manager);
    gtk_box_append(GTK_BOX(sidebar), chat_item);

    //Перший чат за замовчуванням
    chat_data_t *first_chat = g_hash_table_lookup(chat_manager->chats, GINT_TO_POINTER(contact_id[0]));
    if (first_chat != NULL) {
        gtk_box_append(GTK_BOX(chat_area_background), first_chat->messages_container_wrapper);
        chat_manager->active_chat = first_chat;
    }

    GTK_data->sidebar = sidebar;
    GTK_data->chat_manager = chat_manager;

    // Add new group button
    GtkWidget *add_group_button = gtk_button_new_with_label("Add new group");
    gtk_widget_add_css_class(add_group_button, "add-group-button");
    gtk_box_append(GTK_BOX(sidebar), add_group_button);

    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_widget_add_css_class(send_button, "send-button");
    // Connect the callback to the send button
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_clicked), GTK_data);    
    // In the on_activate function, after creating message_entry, add:
    g_signal_connect(message_entry, "activate", G_CALLBACK(on_entry_activated), GTK_data);    
    gtk_box_append(GTK_BOX(input_box), send_button);

    // Layout setup
    gtk_grid_attach(GTK_GRID(main_grid), chat_header, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), input_box, 1, 2, 1, 1);

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

    if (!GTK_data->in_start) {
        cJSON *command17 = cJSON_CreateObject();
        cJSON_AddNumberToObject(command17, "command_code", 17);
        send_and_delete_json(GTK_data->call_data->ssl, &command17);
        GTK_data->in_start = true;
    }
}

void GTK_start(call_data_t *call_data) {
    static GtkApplication *app = NULL;

    if (app == NULL) {
        GTK_data_t *GTK_data = (GTK_data_t *)malloc(sizeof(GTK_data_t));
        GTK_data->message = NULL;
        GTK_data->call_data = call_data;
        GTK_data->message_entry = NULL;
        GTK_data->in_start = false;
        pthread_mutex_init(&GTK_data->message_mutex, NULL); 
        
        app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_NON_UNIQUE);
        g_signal_connect(app, "activate", G_CALLBACK(on_activate), GTK_data);
    }

    g_application_run(G_APPLICATION(app), 0, NULL);
}
