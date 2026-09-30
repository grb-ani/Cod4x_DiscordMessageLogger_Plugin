#include "pinc.h"
#include "Queue.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <memory>

Queue::Queue() {
	memset(&dMessageQueueAttr, 0, sizeof(dMessageQueueAttr));
	dMessageQueueAttr.mq_flags = 0;
	dMessageQueueAttr.mq_maxmsg = 10;
	dMessageQueueAttr.mq_msgsize = MAX_SIZE;
	dMessageQueueAttr.mq_curmsgs = 0;

	discordMessageQueue = mq_open(COD4X_QUEUE_NAME, O_CREAT | O_WRONLY, 0666, &dMessageQueueAttr);
	if(discordMessageQueue == (mqd_t)-1) {
		Plugin_PrintError("Error occurred on opening query: %s\n", strerror(errno));
		exit(EXIT_FAILURE);

	}

}

void Queue::sendQueue(const char* msg) {
	Plugin_Printf("Discord Message Logger Plugin: NOTICE: Sending message: %s\n", msg);

	if(strlen(msg) * sizeof(char) > MAX_SIZE) {
		Plugin_PrintError("Discord Message Logger Plugin: Size of message exceeds a MB, returning...\n");
		return;

	}

	rValue = mq_send(discordMessageQueue, msg, (strlen(msg) + 1) * sizeof(char), 0);
	if(rValue == -1) {
		Plugin_PrintError("Discord Message Logger Plugin: Error occurred on sending message: %s\n", strerror(errno));
		return;

	}

}

Queue::~Queue() {
	mq_close(discordMessageQueue);

}
