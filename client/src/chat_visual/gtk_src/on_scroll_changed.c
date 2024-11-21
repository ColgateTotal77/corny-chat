#include "GTK.h"

void on_scroll_changed(GtkAdjustment *adjustment, gpointer user_data) {
    scroll_data_t *scroll_data = (scroll_data_t *)user_data;

    double value = gtk_adjustment_get_value(adjustment);      // Поточне становище скролла
    double upper = gtk_adjustment_get_upper(adjustment);      // Максимальне становище скролла
    double page_size = gtk_adjustment_get_page_size(adjustment); // Висота видимої області

    if (value + page_size >= upper) { // Якщо дійшли до нижнього краю
        cJSON *command12 = cJSON_CreateObject();
        cJSON_AddNumberToObject(command12, "command_code", 18);       
        cJSON_AddNumberToObject(command12, "sender_id", scroll_data->chat->contact_id);
        send_and_delete_json(scroll_data->ssl, &command12);
    }
}

