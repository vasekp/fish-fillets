#ifndef FISH_FILLETS_IOS_GLUE_H
#define FISH_FILLETS_IOS_GLUE_H

#include "common.h"
#include "input.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <variant>

struct QuitMessage { };

struct FocusMessage {
    bool foreground;
};

using Message = std::variant<QuitMessage, FocusMessage>;

class IOSInstance;

class Glue {
public:
    Glue(void* metalLayer);
    ~Glue();

    void closeActivity();
    void submitMessage(const Message& message);
    IOSInput& input();

private:
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    Message m_message;
    std::atomic<bool> m_msgQueued;

    IOSInstance* m_instance;

    void* m_layer;
    bool m_running;

    void worker();
    void dispatch(const QuitMessage& msg);
    void dispatch(const FocusMessage& msg);
};

#endif //FISH_FILLETS_IOS_GLUE_H
