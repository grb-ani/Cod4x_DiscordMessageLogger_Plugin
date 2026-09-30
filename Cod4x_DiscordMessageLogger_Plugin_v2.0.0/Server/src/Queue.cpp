#include "Queue.hpp"
#include "DiscordBot.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>

IDiscordBot* Queue::botInstance = nullptr;

Queue::Queue() {
    memset(&dMessageQueueAttr, 0, sizeof(dMessageQueueAttr));
    dMessageQueueAttr.mq_flags = 0;
    dMessageQueueAttr.mq_maxmsg = 10;
    dMessageQueueAttr.mq_msgsize = MAX_SIZE;
    dMessageQueueAttr.mq_curmsgs = 0;

    // Ensure clean queue
    mq_unlink(COD4X_QUEUE_NAME);

    discordMessageQueue = mq_open(
        COD4X_QUEUE_NAME,
        O_CREAT | O_RDONLY | O_NONBLOCK,
        0666,
        &dMessageQueueAttr
    );

    if (discordMessageQueue == (mqd_t)-1) {
        fprintf(stderr, "Error opening MQ: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct mq_attr attr;
    memset(&attr, 0, sizeof(attr));
    mq_getattr(discordMessageQueue, &attr);
    printf("mq_msgsize after open: %ld\n", attr.mq_msgsize);

    bufferSize = MAX_SIZE * sizeof(char);
    buffer = (char*)malloc(bufferSize);
    memset(buffer, 0, bufferSize);

    printf("NOTICE: MQ opened successfully, switching to event-driven mode.\n");

    registerNotification();
}

void Queue::setBot(IDiscordBot* bot) {
    botInstance = bot;
}

void Queue::registerNotification() {
    struct sigevent sev{};
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &Queue::messageHandler;
    sev.sigev_notify_attributes = nullptr;
    sev.sigev_value.sival_ptr = this;

    if (mq_notify(discordMessageQueue, &sev) == -1) {
        if (errno != EBUSY) {
            fprintf(stderr, "Error registering mq_notify: %s\n", strerror(errno));
        }
    }
}

void Queue::messageHandler(union sigval sv) {
    Queue* self = static_cast<Queue*>(sv.sival_ptr);

    memset(self->buffer, 0, self->bufferSize);

    ssize_t msgLen = mq_receive(
        self->discordMessageQueue,
        self->buffer,
        self->bufferSize,
        nullptr
    );

    if (msgLen >= 0) {
        self->buffer[msgLen] = '\0';
        printf("NOTICE: Returning message: %s\n", self->buffer);

        if (botInstance) {
            botInstance->log(self->buffer);
        }

    } else {
        if (errno != EAGAIN) {
            fprintf(stderr, "Error receiving MQ message: %s\n", strerror(errno));
        }
    }

    self->registerNotification();
}

Queue::~Queue() {
    free(buffer);
    mq_close(discordMessageQueue);
}
