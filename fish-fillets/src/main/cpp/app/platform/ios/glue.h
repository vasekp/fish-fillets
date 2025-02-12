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
    bool focus;
};

struct ResizeMessage {
    USize size;
};

using Message = std::variant<QuitMessage, FocusMessage, ResizeMessage>;

class IOSInstance;

class Glue {
public:
    Glue(void* metalLayer);
    ~Glue();

    void submitMessage(const Message& message);
    IOSInstance& instance();
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
    void dispatch(const ResizeMessage& msg);
};

#endif //FISH_FILLETS_IOS_GLUE_H
