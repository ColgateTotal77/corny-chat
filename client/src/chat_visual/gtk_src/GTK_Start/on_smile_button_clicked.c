#include "GTK.h"

void on_sticker_selected(GtkButton *button, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    const char *sticker = g_object_get_data(G_OBJECT(button), "sticker");
    
    if (sticker && GTK_data && GTK_data->message_entry) {
        // Get the current text from the message entry
        const char *current_text = gtk_editable_get_text(GTK_EDITABLE(GTK_data->message_entry));

        // Append the selected emoji to the current text
        char *new_text = g_strdup_printf("%s%s", current_text, sticker);
        gtk_editable_set_text(GTK_EDITABLE(GTK_data->message_entry), new_text);

        // Free the dynamically allocated string
        g_free(new_text);
    }
}

void on_smile_button_clicked(GtkButton *button, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;
    
    // Create popover
    GtkWidget *popover = gtk_popover_new();
    gtk_widget_add_css_class(popover, "sticker-popover");
    
    // Create scrolled window for large number of stickers
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, 300, 400);
    
    // Create grid for stickers
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_add_css_class(grid, "sticker-grid");
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    
    // Extended array of emoji/stickers
    const char *stickers[] = {
        // Smileys & Emotion
        "😀", "😃", "😄", "😁", "😅", "😂", "🤣", "😊", "😇", "🙂", "🙃", "😉", "😌", "😍", "🥰",
        "😘", "😗", "😙", "😚", "😋", "😛", "😝", "😜", "🤪", "🤨", "🧐", "🤓", "😎", "🥸", "🤩",
        "🥳", "😏", "😒", "😞", "😔", "😟", "😕", "🙁", "☹️", "😣", "😖", "😫", "😩", "🥺", "😢",
        
        // Hearts & Love
        "❤️", "🧡", "💛", "💚", "💙", "💜", "🖤", "🤍", "🤎", "💕", "💞", "💓", "💗", "💖", "💘",
        
        // Hand Gestures
        "👍", "👎", "👊", "✊", "🤛", "🤜", "🤚", "👋", "🤟", "✌️", "🤘", "👌", "🤌", "🤏", "👈",
        
        // Activities & Objects
        "🎮", "🎲", "🎭", "🎨", "🎼", "🎵", "🎶", "🎤", "🎧", "🎸", "🎹", "🎺", "🎻", "🪘", "🥁",
        
        // Nature & Weather
        "🌞", "🌝", "🌛", "🌜", "🌚", "🌕", "🌖", "🌗", "🌘", "🌑", "🌒", "🌓", "🌔", "⭐", "🌟",
        
        // Animals
        "🐶", "🐱", "🐭", "🐹", "🐰", "🦊", "🐻", "🐼", "🐨", "🐯", "🦁", "🐮", "🐷", "🐸", "🐵",
        
        // Food & Drink
        "🍎", "🍐", "🍊", "🍋", "🍌", "🍉", "🍇", "🍓", "🫐", "🍈", "🍒", "🍑", "🥭", "🍍", "🥥",
        
        // Symbols & Flags
        "✨", "💫", "💥", "💢", "💦", "💨", "🕊️", "🦋", "🌈", "💝", "💟", "☮️", "✝️", "☪️", "🕉️"
    };
    
    // Add stickers to grid
    int row = 0, col = 0;
    for (size_t i = 0; i < G_N_ELEMENTS(stickers); i++) {
        GtkWidget *sticker_button = gtk_button_new_with_label(stickers[i]);
        gtk_widget_add_css_class(sticker_button, "sticker-button");
        g_object_set_data(G_OBJECT(sticker_button), "sticker", (gpointer)stickers[i]);
        g_signal_connect(sticker_button, "clicked", G_CALLBACK(on_sticker_selected), GTK_data);
        
        gtk_grid_attach(GTK_GRID(grid), sticker_button, col, row, 1, 1);
        
        col++;
        if (col >= 7) { 
            col = 0;
            row++;
        }
    }
    
    // Add grid to scrolled window
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), grid);
    
    // Add scrolled window to popover
    gtk_popover_set_child(GTK_POPOVER(popover), scrolled_window);
    gtk_widget_set_parent(popover, GTK_WIDGET(button));
    
    gtk_popover_popup(GTK_POPOVER(popover));
}
