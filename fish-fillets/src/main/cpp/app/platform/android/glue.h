#ifndef FISH_FILLETS_ANDROID_GLUE_H
#define FISH_FILLETS_ANDROID_GLUE_H

#include "common.h"
#include "input.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <variant>
#include <jni.h>
#include <android/native_window_jni.h>

struct QuitMessage { };

struct FocusMessage {
    bool foreground;
};

struct SurfaceMessage {
    ANativeWindow* surface;
};

using Message = std::variant<QuitMessage, FocusMessage, SurfaceMessage>;

class AndroidInstance;

class Glue {
public:
    Glue(JNIEnv* env, jobject activity, jobject jAssets, std::string&& userPath);
    ~Glue();

    void closeActivity();
    void submitMessage(const Message& message);

    AndroidInput& input();

private:
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    Message m_message;
    std::atomic<bool> m_msgQueued;

    JNIEnv* m_mainEnv;
    jobject m_activity;
    jobject m_assets;
    AndroidInstance* m_instance;
    ANativeWindow* m_window;

    bool m_running;

    void worker(JavaVM* vm, std::string&& userPath);
    void dispatch(const QuitMessage& msg);
    void dispatch(const FocusMessage& msg);
    void dispatch(const SurfaceMessage& msg);
};

#endif //FISH_FILLETS_ANDROID_GLUE_H
