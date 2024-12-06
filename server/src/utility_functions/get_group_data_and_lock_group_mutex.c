#include "server.h"


bool get_group_data_and_lock_group_mutex(call_data_t *call_data, int group_id, 
                                         chat_t **chat_data) {
    printf("MUTEX LOG: get_group_data_and_lock_group_mutex<--------------------------------\n");
    fflush(stdout);

    printf("MUTEX LOG: lock(call_data->general_data->chats_mutex)\n");
    fflush(stdout);
    // Critical resource access: CHATS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->chats_mutex);

    chat_t *chat = ht_get(call_data->general_data->chats, group_id);

    if (chat == NULL) {
        pthread_mutex_unlock(call_data->general_data->chats_mutex);
        // Critical resource access: CHATS HASH TABLE. Possible end
        printf("MUTEX LOG: unlock(call_data->general_data->chats_mutex)\n");
        fflush(stdout);

        return false;
    }

    pthread_mutex_t *chat_mutex = &chat->mutex;

    pthread_mutex_unlock(call_data->general_data->chats_mutex);
    // Critical resource access: CHATS HASH TABLE. End
    printf("MUTEX LOG: unlock(call_data->general_data->chats_mutex)\n");
    fflush(stdout);

    printf("MUTEX LOG: lock(chat_mutex)\n");
    fflush(stdout);
    // Critical resource access: SELECTED CHAT. Start
    int rc = pthread_mutex_lock(chat_mutex);

    if (rc == EINVAL) {
        return false;
    }

    if (chat == NULL) {
        pthread_mutex_unlock(chat_mutex);
        // Critical resource access: SELECTED CHAT. Possible end
        printf("MUTEX LOG: unlock(chat_mutex)\n");
        fflush(stdout);

        return false;
    }

    *(chat_data) = chat;

    return true;
}
