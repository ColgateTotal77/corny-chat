#include <gtk/gtk.h>

void apply_css(GtkWidget *widget, char* path) {
    (void)widget;
    GtkCssProvider *css_provider = gtk_css_provider_new();

    // Load CSS with only two parameters
    gtk_css_provider_load_from_path(css_provider, path);

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(css_provider);
}

void clear_css(void) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    
    gtk_style_context_remove_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider)
    );
    
    // Освобождаем память
    g_object_unref(css_provider);
}

