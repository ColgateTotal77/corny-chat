#include "GTK.h"

void profile_css(GtkWidget *widget) {
    (void)widget;
    GtkCssProvider *css_provider = gtk_css_provider_new();

    // Load CSS with only two parameters
    gtk_css_provider_load_from_path(css_provider, "src/chat_visual/gtk_src/GTK_Start/style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(css_provider);
}
