#include "GTK.h"

gboolean waiting_window_wrapper(gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t *)(user_data);
    GTK_data->wait = waiting_window(GTK_data);
    return G_SOURCE_REMOVE;
}

static gboolean on_delete_event(GtkWindow *dialog, gpointer user_data) {
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;
    (void)dialog;
    on_main_window_destroy(GTK_data);
    *(GTK_data->call_data->stop_flag) = true;
    return false;
}

GtkWidget *waiting_window(GTK_data_t *GTK_data) {
    gtk_init();

    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Server disconnected!");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(GTK_data->window));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);

    GtkWidget *label = gtk_label_new("Wait...");
    gtk_box_append(GTK_BOX(box), label);

    gtk_window_set_child(GTK_WINDOW(dialog), box);
    gtk_window_present(GTK_WINDOW(dialog));

    g_signal_connect(dialog, "close-request", G_CALLBACK(on_delete_event), GTK_data);
    return dialog;
}


