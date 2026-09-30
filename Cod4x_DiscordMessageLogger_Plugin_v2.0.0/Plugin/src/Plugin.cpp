#include "pinc.h"
#include "Queue.hpp"
#include "version.h"

#include <pthread.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

pthread_mutex_t mutex;
IQueue* iQueue = nullptr;
const size_t MAX_SAY_TEXT = 150;

PCL int OnInit() {
	pthread_mutex_init(&mutex, NULL);

	iQueue = new Queue();

	return 0;
}

PCL void OnMessageSent(char* message, int slot, qboolean *show, int mode) {
    pthread_mutex_lock(&mutex);

    // Ignore quickchat messages (0x14)
    if ((unsigned char)message[0] == 0x14) {
        pthread_mutex_unlock(&mutex);
        return;
    }

    // Make a local copy so we never modify COD4X's shared buffer
    char local_msg[MAX_SAY_TEXT + 1];
    snprintf(local_msg, sizeof(local_msg), "%s", message);

    // Strip leading control codes
    char* msg_ptr = local_msg;
    while ((unsigned char)msg_ptr[0] < 0x20) {
        msg_ptr++;
    }

    char* playerName = Plugin_GetPlayerName(slot);

    // Build JSON payload
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "player", playerName);
    cJSON_AddStringToObject(root, "message", msg_ptr);
    cJSON_AddNumberToObject(root, "slot", slot);
    cJSON_AddBoolToObject(root, "show", *show);
    cJSON_AddNumberToObject(root, "mode", mode);

    char* jsonStr = cJSON_PrintUnformatted(root);

    iQueue->sendQueue(jsonStr);

    free(jsonStr);
    cJSON_Delete(root);

    pthread_mutex_unlock(&mutex);
}

PCL void OnTerminate() {
	pthread_mutex_destroy(&mutex);

	delete iQueue;

}

PCL void OnInfoRequest(pluginInfo_t *info){

    info->handlerVersion.major = PLUGIN_HANDLER_VERSION_MAJOR;
    info->handlerVersion.minor = PLUGIN_HANDLER_VERSION_MINOR;

    info->pluginVersion.major = Cod4x_DiscordMessageLogger_VERSION_MAJOR;
    info->pluginVersion.minor = Cod4x_DiscordMessageLogger_VERSION_MINOR;
    strncpy(info->fullName,"Cod4x Discord Logger Plugin",sizeof(info->fullName));
    strncpy(info->shortDescription,"Discord Message Logger Plugin for Cod4x.",sizeof(info->shortDescription));
    strncpy(info->longDescription,"This plugin is used to send server chat messages to a discord channel. Coded my LM40 ( DevilHunter )",sizeof(info->longDescription));
}


