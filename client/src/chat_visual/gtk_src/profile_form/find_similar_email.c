#include "GTK.h"

// Find the most similar email based on the prefix
const char *find_similar_email(const char *input, GtkListBox *list_box) {
    if (!input || !list_box) {
        return NULL;
    }

    GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(list_box));
    const char *best_match = NULL;

    while (child) {
        if (GTK_IS_LIST_BOX_ROW(child)) {
            GtkWidget *row_child = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(child));
            if (GTK_IS_LABEL(row_child)) {  // Check if it's directly a label
                const char *row_text = gtk_label_get_text(GTK_LABEL(row_child));
                
                // If the input is a prefix of the current row text
                if (row_text && g_str_has_prefix(row_text, input)) {
                    best_match = row_text;
                    break;  // Found a match, exit loop
                }
            }
        }
        child = gtk_widget_get_next_sibling(child);
    }

    return best_match;
}
