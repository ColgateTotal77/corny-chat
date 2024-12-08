#include "GTK.h"

// Helper function to wrap text to a specified width
char *wrap_text_to_width(const char *message_text, int max_width, PangoContext *context) {
    // Create a PangoLayout to measure and wrap the text
    PangoLayout *layout = pango_layout_new(context);
    pango_layout_set_width(layout, max_width * PANGO_SCALE); // Set max width in Pango units
    pango_layout_set_text(layout, message_text, -1);
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);

    // Extract the lines of wrapped text
    GString *wrapped_text = g_string_new(NULL);
    PangoLayoutIter *iter = pango_layout_get_iter(layout);

    int line_count = pango_layout_get_line_count(layout);
    int current_line = 0;

    while (pango_layout_iter_get_line_readonly(iter) != NULL) {
        PangoLayoutLine *line = pango_layout_iter_get_line_readonly(iter);
        const gchar *start = pango_layout_get_text(layout) + line->start_index;
        int length = line->length;

        g_string_append_len(wrapped_text, start, length);
        
        // Add newline only if it's not the last line
        current_line++;
        if (current_line < line_count) {
            g_string_append_c(wrapped_text, '\n');
        }

        // Move to next line
        if (!pango_layout_iter_next_line(iter)) {
            break;
        }
    }
    pango_layout_iter_free(iter);

    char *result = g_string_free(wrapped_text, FALSE);
    g_object_unref(layout);
    return result;
}

void prepare_message_label(GtkWidget *message_label, const char *message_text) {
    // Obtain the PangoContext from the widget
    PangoContext *context = gtk_widget_get_pango_context(message_label);

    // Wrap text programmatically to fit a width of 450px
    char *wrapped_text = wrap_text_to_width(message_text, 450, context);

    // Set the label text
    gtk_label_set_text(GTK_LABEL(message_label), wrapped_text);

    // Free the dynamically allocated wrapped text
    free(wrapped_text);

    // Enable wrapping
    gtk_label_set_wrap(GTK_LABEL(message_label), TRUE);

    // Set wrap mode for better word handling
    gtk_label_set_wrap_mode(GTK_LABEL(message_label), PANGO_WRAP_WORD_CHAR);

    // Add a CSS class for styling
    gtk_widget_add_css_class(message_label, "message-text");
}
