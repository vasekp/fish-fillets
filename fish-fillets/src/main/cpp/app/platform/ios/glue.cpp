#include "glue.h"
#include "log.h"
#include "iosinstance.h"
#include "subsystem/rng.h"
#include "subsystem/graphics/graphicsbackend.h"
#include "subsystem/audio.h"
#include "bridge.h"

Glue::Glue(void* metalLayer) :
    m_msgQueued(false),
    m_layer(metalLayer),
    m_running(false)
{
    m_thread = std::thread{[&]() { worker(); }};
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [this]() { return m_running; });
    Log::info<Log::lifecycle>("thread running (main)");
}

Glue::~Glue() {
    submitMessage(QuitMessage{});
    m_thread.join();
    Log::info<Log::lifecycle>("thread quitted (main)");
}

void Glue::submitMessage(const Message& message) {
    m_message = message;
    m_msgQueued.store(true, std::memory_order::release);
    m_cond.notify_one();
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [this]() { return !m_msgQueued.load(std::memory_order::relaxed); });
}

/*void Glue::closeActivity() {
    auto& env = m_instance->jni;
    jclass clazz = env->GetObjectClass(m_activity);
    jmethodID method = env->GetMethodID(clazz, "finish", "()V");
    env->CallVoidMethod(m_activity, method);
}*/

void Glue::worker() {
    IOSInstance instance{*this};
    m_instance = &instance;

    m_running = true;
    Log::info<Log::lifecycle>("thread running (worker)");
    m_cond.notify_one();
    
    auto display = vulkan::Display(m_layer);
    m_instance->graphics().activate(std::move(display));
    m_instance->live = true;
    m_instance->run();

    while(m_running) {
        try {
            if(!instance.live) {
                std::unique_lock lock(m_mutex);
                m_cond.wait(lock, [this]() { return m_msgQueued.load(std::memory_order::relaxed); });
            }

            if(m_msgQueued.load(std::memory_order::acquire)) {
                std::visit([this](auto&& arg) { this->dispatch(arg); }, m_message);
                m_msgQueued.store(false, std::memory_order::release);
                m_cond.notify_one();
            }
            m_instance->inputSource().deliverEvents();

            if(instance.live && instance.running())
                instance.updateAndDraw();
        } catch(std::exception& e) {
            Log::error("Caught exception: %s", e.what());
        }
    }
    Log::info<Log::lifecycle>("thread quitting (worker)");
    instance.quit();
}

void Glue::dispatch(const QuitMessage&) {
    m_running = false;
}

void Glue::dispatch(const FocusMessage& msg) {
    if(!m_instance->live)
        return;
    if(msg.foreground)
        m_instance->run();
    else
        m_instance->pause();
}

#if 0
void Glue::dispatch(const SurfaceMessage& msg) {
    if(msg.surface != nullptr) {
        if(msg.surface == m_window) {
            Log::debug<Log::graphics>("surface updated (worker)");
            m_instance->graphics().notifyResize({(unsigned)ANativeWindow_getWidth(msg.surface), (unsigned)ANativeWindow_getHeight(msg.surface)});
        } else {
            Log::debug<Log::graphics>("new surface (worker)");
            m_window = msg.surface;
            auto display = ogl::Display(m_window);
            m_instance->graphics().activate(std::move(display));
            m_instance->live = true;
            m_instance->run();
        }
    } else {
        Log::debug<Log::graphics>("surface deleted (worker)");
        m_window = nullptr;
        m_instance->graphics().shutdown();
        m_instance->live = false;
    }
}
#endif

IOSInstance& Glue::instance() {
    return *m_instance;
}

IOSInput& Glue::input() {
    return m_instance->inputSource();
}

void* start(void* metalLayer) {
    Log::debug<Log::platform>("start: ", metalLayer);
    return new Glue(metalLayer);
}

void touchEvent(void* pGlue, int type, float x, float y) {
    auto& glue = *(Glue*)pGlue;
    Log::debug<Log::platform>("touch: type ", type, " @ ", FCoords{x, y});
    glue.input().registerTouchEvent(type, {x, y});
}

void renderAudio(void* pGlue, unsigned count, void* buffer) {
    auto& glue = *(Glue*)pGlue;
    glue.instance().audio().mix(reinterpret_cast<float*>(buffer), count);
}

#if 0
extern "C"
JNIEXPORT void JNICALL
Java_cz_absolutno_fillets_MainActivity_setForeground(JNIEnv* env, jobject thiz, jint handle, jboolean focus) {
    Log::verbose<Log::lifecycle>("onFocus (C++), handle = ", handle, " focus = ", (bool)focus);
    auto& glue = *global_map.at(handle);
    glue.submitMessage(FocusMessage{(bool)focus});
}

extern "C"
JNIEXPORT void JNICALL
Java_cz_absolutno_fillets_MainActivity_setSurface(JNIEnv* env, jobject, jint handle, jobject jSurface) {
    Log::verbose<Log::lifecycle>("setSurface (C++), handle = ", handle, " surface = ", (void*)jSurface);
    auto& glue = *global_map.at(handle);
    glue.submitMessage(SurfaceMessage{jSurface ? ANativeWindow_fromSurface(env, jSurface) : nullptr});
}
#endif
