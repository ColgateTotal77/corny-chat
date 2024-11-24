#include "GTK.h"

void on_scroll_changed(GtkAdjustment *adjustment, gpointer user_data) {
    scroll_data_t *scroll_data = (scroll_data_t *)user_data;

    double value = gtk_adjustment_get_value(adjustment); // Поточне становище скролла
    double upper = gtk_adjustment_get_upper(adjustment); // Максимальне становище скролла
    double lower = gtk_adjustment_get_lower(adjustment); // Мінімальне становище скрола становище скролла
    double page_size = gtk_adjustment_get_page_size(adjustment); // Висота видимої області

    if (value + page_size >= upper) { // Якщо дійшли до нижнього краю
        mark_chat_msgs_as_readed(scroll_data->ssl, scroll_data->chat->contact_id);
    }
    else if (value <= lower) {
        get_num_of_msgs_with_user(scroll_data->ssl, scroll_data->chat->contact_id, scroll_data->chat->last_message_id, 20);
    }
}
