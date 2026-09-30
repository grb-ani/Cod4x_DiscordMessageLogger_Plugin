#pragma once

#include <mqueue.h>
#include <signal.h>
#include <memory>

#define MAX_SIZE 1024
#define COD4X_QUEUE_NAME "/libCod4xDiscordMessageQ"

class IDiscordBot; // forward declaration

class Queue {
public:
    Queue();
    ~Queue();

    static void setBot(IDiscordBot* bot);

private:
    mqd_t discordMessageQueue;
    struct mq_attr dMessageQueueAttr;

    char* buffer = nullptr;
    size_t bufferSize = 0;

    static IDiscordBot* botInstance;

    static void messageHandler(union sigval sv);
    void registerNotification();
};
