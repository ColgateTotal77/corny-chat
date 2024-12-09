#include "GTK.h"

void on_scroll_changed(GtkAdjustment *adjustment, gpointer user_data) {
    scroll_data_t *scroll_data = (scroll_data_t *)user_data;

    gdouble value = gtk_adjustment_get_value(adjustment);
    gdouble upper = gtk_adjustment_get_upper(adjustment);
    gdouble lower = gtk_adjustment_get_lower(adjustment);
    gdouble page_size = gtk_adjustment_get_page_size(adjustment);

    if (value + page_size >= upper) {
        if(scroll_data->chat->is_group) {
            mark_group_msgs_as_readed(scroll_data->ssl, scroll_data->chat->contact_id);
        }
        else {
            mark_chat_msgs_as_readed(scroll_data->ssl, scroll_data->chat->contact_id);
        }
    }
    else if (value <= lower) {
        if(scroll_data->chat->is_group) {
            get_num_of_msgs_from_group(scroll_data->ssl, scroll_data->chat->contact_id, scroll_data->chat->last_message_id, 15);
        }
        else {
            get_num_of_msgs_with_user(scroll_data->ssl, scroll_data->chat->contact_id, scroll_data->chat->last_message_id, 15);
        }
    }
}
