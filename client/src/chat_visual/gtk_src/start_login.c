#include "GTK.h"

// Function prototypes
char* get_name(void);
bool check_password(char *password);

/* Function to convert login and password to JSON and print */
static void print_json_data(const char *login, const char *password, call_data_t *call_data) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "name", login);
    cJSON_AddStringToObject(json, "password", password);

    send_and_delete_json(call_data->ssl, &json);
}

bool check_password(char *password) { //Валідація паролю
    if (password == NULL) { //Перевірка на порожній рядок
        return false;
    }

    int password_length = mx_strlen(password);

    if (password_length < 8 || password_length > 20) { //Перевірка на довжину, яка має бути більше 7 і менше 20
        return false;
    }

    for (int i = 0; i < password_length; i++) {
        if (mx_check_space(password[i]) || password[i] < 33 || password[i] > 126) { //Перевірка пароля на невалідні символи по ASCII, тут дозволені від '!' до '~'
            return false;
        }
    }
    return true;
}

void set_error_text(GtkWidget *error_label, bool *error, const char *error_text) {
    gtk_label_set_text(GTK_LABEL(error_label), error_text);
    if (error) {
        *error = true;
    }
}

static void on_window_destroy(GtkWindow *window, gpointer user_data) {
    //(void)window;
    GtkWidget **entries = (GtkWidget **)user_data;
    GTK_data_t *GTK_data = (GTK_data_t *)entries[3];
    if(!GTK_data->login_successful){
        pthread_mutex_destroy(&GTK_data->pthread_mutex);
        pthread_cond_destroy(&GTK_data->pthread_cond);
        *(GTK_data->stop_login) = true;
        //pthread_join(GTK_data->recv_thread, NULL);

        *(GTK_data->call_data->stop_flag) = true;
        free(GTK_data);
        GTK_data = NULL;
        g_free(entries);
        printf("\nclosing the window\n");
        gtk_window_destroy(window);
    }
}


/* Callback function to handle the login button click */
static void on_login_button_clicked(GtkWidget *button, gpointer user_data) {
    (void)button;
    GtkWidget **entries = (GtkWidget **)user_data;
    const char *login_input = gtk_editable_get_text(GTK_EDITABLE(entries[0]));
    const char *password_input = gtk_editable_get_text(GTK_EDITABLE(entries[1]));
    GtkWidget *error_label = entries[2];
    GTK_data_t *GTK_data = (GTK_data_t *)entries[3];

    // Check login validity: length 2-30 characters
    bool valid_login = login_input && strlen(login_input) >= 2 && strlen(login_input) <= 30;

    // Check password validity using the check_password function
    bool valid_password = password_input && check_password((char *)password_input);

    // Reset error label text
    gtk_label_set_text(GTK_LABEL(error_label), "");
    bool is_error_appear = false;

    // Determine the error message based on validation results
    if (!valid_login && !valid_password) {
        set_error_text(error_label, &is_error_appear, "Both login and password are invalid.");
    } else if (!valid_login) {
        set_error_text(error_label, &is_error_appear, "Invalid login. Must be 2-30 characters.");
    } else if (!valid_password) {
        set_error_text(error_label, &is_error_appear, "Invalid password. Must be 8-20 characters with no spaces.");
    } else {
        // If validation passes, print JSON data
        print_json_data(login_input, password_input, GTK_data->call_data);

        pthread_mutex_lock(&GTK_data->pthread_mutex);

        // Wait for the login response
        pthread_cond_wait(&GTK_data->pthread_cond, &GTK_data->pthread_mutex);
        pthread_mutex_unlock(&GTK_data->pthread_mutex);

        if(GTK_data->login_successful){
            GtkWidget *window = entries[4];
            check_remember_me(entries[5], login_input, password_input);
            gtk_widget_set_visible(window, FALSE);
            pthread_mutex_destroy(&GTK_data->pthread_mutex);
            pthread_cond_destroy(&GTK_data->pthread_cond);
            GTK_start(GTK_data);
            g_free(entries);
            gtk_window_close(GTK_WINDOW(window));
        }else{
            set_error_text(error_label, &is_error_appear, "Login or password incorrect. Please try again.");
        }
    }

    // Add red color style to the error label if there's an error
    if (is_error_appear) {
        gtk_widget_add_css_class(error_label, "error-label");
    } else {
        gtk_widget_remove_css_class(error_label, "error-label");
    }
}

void on_activate(GtkApplication *app, gpointer GTK_data) {
    GTK_data_t *GTK_data1 = (GTK_data_t*)GTK_data;
    
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);

    // To center the window in GTK4
    gtk_window_set_transient_for(GTK_WINDOW(window), NULL);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    
    GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(main_container, 40);
    gtk_widget_set_margin_end(main_container, 40);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_vexpand(vbox, TRUE);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
    
    // Login field container
    GtkWidget *login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *login_label = gtk_label_new("Login:");
    gtk_widget_add_css_class(login_label, "input-label");
    gtk_widget_set_halign(login_label, GTK_ALIGN_START);
    GtkWidget *login_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_entry), "Enter login");
    gtk_widget_set_hexpand(login_entry, TRUE);
    gtk_box_append(GTK_BOX(login_box), login_label);
    gtk_box_append(GTK_BOX(login_box), login_entry);

    // Password field container
    GtkWidget *password_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_add_css_class(password_label, "input-label");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_widget_set_hexpand(password_entry, TRUE);
    gtk_box_append(GTK_BOX(password_box), password_label);
    gtk_box_append(GTK_BOX(password_box), password_entry);

    // Error label for displaying login/password issues
    GtkWidget *error_label = gtk_label_new("");
    gtk_widget_add_css_class(error_label, "error-label");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(password_box), error_label);

    // Remember me checkbox and login button
    GtkWidget *remember_me_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *remember_me_check = gtk_check_button_new_with_label("Remember me");
    gtk_widget_add_css_class(remember_me_check, "remember-me-checkbox");
    gtk_widget_set_hexpand(remember_me_check, TRUE);

    input_saved_data(login_entry, password_entry, remember_me_check);

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    gtk_widget_add_css_class(login_button, "login-button");

    // Store login, password entries and error label for callback
    GtkWidget **entries = g_malloc(sizeof(GtkWidget *) * 6);
    entries[0] = login_entry;
    entries[1] = password_entry;
    entries[2] = error_label;
    entries[3] = GTK_data;
    entries[4] = window;
    entries[5] = remember_me_check;

    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), entries);

    gtk_box_append(GTK_BOX(remember_me_box), remember_me_check);
    gtk_box_append(GTK_BOX(remember_me_box), login_button);
    
    gtk_box_append(GTK_BOX(vbox), login_box);
    gtk_box_append(GTK_BOX(vbox), password_box);
    gtk_box_append(GTK_BOX(vbox), remember_me_box);
    
    gtk_box_append(GTK_BOX(main_container), vbox);
    
    apply_css("src/chat_visual/gtk_src/GTK_Start/style.css");

    gtk_window_set_child(GTK_WINDOW(window), main_container);
    gtk_window_present(GTK_WINDOW(window));

    if (pthread_create(&GTK_data1->recv_thread, NULL, &recv_login_msg_handler, (void*)GTK_data) != 0) {
        printf("ERROR: pthread\n");
    }
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), entries);
}

void start_login(call_data_t *call_data) {
    // Use a static variable to track the application instance
    static GtkApplication *app = NULL;
    
    // If no app exists, create a new one
    if (app == NULL) {
        GTK_data_t *GTK_data = (GTK_data_t *)malloc(sizeof(GTK_data_t));
        GTK_data->call_data = call_data;
        GTK_data->login_successful = false;
        pthread_mutex_init(&GTK_data->pthread_mutex, NULL);
        pthread_cond_init(&GTK_data->pthread_cond, NULL);

        app = gtk_application_new("com.example.GtkApplication1", G_APPLICATION_NON_UNIQUE);
        g_signal_connect(app, "activate", G_CALLBACK(on_activate), GTK_data);
    }
    
    // Activate the application
    g_application_run(G_APPLICATION(app), 0, NULL);
    
    // Optional: Uncomment if you want to reset the app after closing
    // g_object_unref(app);
    // app = NULL;
}
