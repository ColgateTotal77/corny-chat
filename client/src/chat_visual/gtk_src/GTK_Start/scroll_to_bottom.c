#include <gtk/gtk.h>

gboolean scroll_idle_callback(gpointer data) {
    GtkAdjustment *adj = GTK_ADJUSTMENT(data);
    // Устанавливаем значение прокрутки в самый низ
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
    return G_SOURCE_REMOVE; // Удаляем функцию из цикла событий
}

void scroll_to_bottom(GtkWidget *container) {
    if (!container) return;

    // Ищем родительский GtkScrolledWindow
    GtkWidget *scrolled = gtk_widget_get_ancestor(container, GTK_TYPE_SCROLLED_WINDOW);
    if (scrolled) {
        // Получаем вертикальную прокрутку
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled));

        // Проверяем, нужно ли прокручивать
        if (gtk_adjustment_get_value(adj) != gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj)) {
            // Выполняем прокрутку
            g_idle_add(scroll_idle_callback, adj);
        }
    }
}
