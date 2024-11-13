//
// Created by konst on 09.11.24.
//

#include "../../inc/sql.h"

/**
 * 
 * @param msg 
 * @param id 
 * @param created_at 
 * @param owner_id 
 * @param target_id 
 * @param target_group_id 
 * @param message 
 * @param readed 
 */
void init_message(s_message *msg, const int id, const char *created_at,
                  const int owner_id, const int target_id, const int target_group_id,
                  char *message,const bool readed) {
				//char message[512],const bool readed) {
	msg->id = id;
	snprintf(msg->createdAt, sizeof(msg->createdAt), "%s", created_at);
	msg->owner_id = owner_id;
	msg->target_id = target_id;
	msg->target_group_id = target_group_id;
	strcpy(msg->message, message);
	//snprintf(msg->message, strlen(message), "%s", message);
	msg->readed = readed;
}
