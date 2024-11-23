#include "create_json.h"
#include "command_codes.h"

cJSON *you_were_added_to_group_notification(int by_who_id, char *by_who_nickname,
                                            int group_id, char *group_name) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", YOU_WERE_ADDED_TO_CHAT);
    cJSON_AddNumberToObject(notification_json, "contact", by_who_id);
    cJSON_AddStringToObject(notification_json, "contact_nickname", by_who_nickname);
    cJSON_AddNumberToObject(notification_json, "chat_id", group_id);
    cJSON_AddStringToObject(notification_json, "chat_name", group_name);

    return notification_json;
}
