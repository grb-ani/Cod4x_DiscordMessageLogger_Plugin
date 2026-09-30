#include "JSON.hpp"
#include "DiscordBot.hpp"
#include "Queue.hpp"
#include "Signal.hpp"
#include "Thread.hpp"

#include <cstddef>
#include <memory>
#include <unistd.h>

int main(int argc, char** argv) {

    bool qRun = true;

    //
    // Load JSON config
    //
    std::shared_ptr<Json::discordData> discordDStruct = std::make_shared<Json::discordData>();
    std::unique_ptr<Json> json = std::make_unique<Json>();
    json->dData = discordDStruct;
    json->readDataToStruct();

    printf("NOTICE: Token is: %s\n", discordDStruct->botToken);
    printf("NOTICE: Channel ID is: %lu\n", discordDStruct->channelID);

    //
    // Create Discord bot
    //
    std::unique_ptr<IDiscordBot> iDiscordBot = std::make_unique<DiscordBot>(
        discordDStruct->botToken,
        discordDStruct->channelID
    );

    //
    // Create Queue (event-driven)
    //
    Queue queue;
    Queue::setBot(iDiscordBot.get());

    //
    // Signal handler
    //
    std::shared_ptr<SigInt> Signal = std::make_shared<SigInt>(&qRun);
    Signal->instance = Signal;
    std::shared_ptr<ISignal> iSignal = std::move(Signal);
    iSignal->regSig();

    //
    // Thread to run Discord bot
    //
    std::unique_ptr<IThread> iThread = std::make_unique<Thread>();

    iThread->setThreadFunc([](void* arg) -> void* {
        IDiscordBot* bot = static_cast<IDiscordBot*>(arg);
        return bot->run(nullptr);
    });

    iThread->run(iDiscordBot.get());

    //
    // Main thread waits for SIGINT
    //
    while (qRun) {
        usleep(100000);
    }

    //
    // Shutdown
    //
    iDiscordBot->stop();
    iThread->stop();

    return 0;
}
