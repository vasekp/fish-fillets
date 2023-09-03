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

void Glue::worker() {
    IOSInstance instance{*this};
    m_instance = &instance;

    m_running = true;
    Log::info<Log::lifecycle>("thread running (worker)");
    m_cond.notify_one();
    
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
            instance.inputSource().deliverEvents();

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
    if(msg.focus)
        m_instance->run();
    else
        m_instance->pause();
}

void Glue::dispatch(const ResizeMessage& msg) {
    if(!m_instance->live) {
        auto display = vulkan::Display(m_layer);
        m_instance->graphics().activate(std::move(display));
        m_instance->live = true;
        m_instance->run();
    }
    m_instance->graphics().notifyResize(msg.size);
}

IOSInstance& Glue::instance() {
    return *m_instance;
}

IOSInput& Glue::input() {
    return m_instance->inputSource();
}

/* Swift bridge functions */

void* startApp(void* metalLayer) {
    Log::debug<Log::platform>("start: ", metalLayer);
    return new Glue(metalLayer);
}

void resizeWin(void* pGlue, unsigned width, unsigned height) {
    auto& glue = *(Glue*)pGlue;
    Log::debug<Log::platform>("Resize: ", width, "x", height);
    glue.submitMessage(ResizeMessage{USize{width, height}});
}

void setFocus(void* pGlue, int focus) {
    auto& glue = *(Glue*)pGlue;
    glue.submitMessage(FocusMessage{(bool)focus});
}

void touchEvent(void* pGlue, int type, float x, float y) {
    auto& glue = *(Glue*)pGlue;
    Log::verbose<Log::platform>("touch: type ", type, " @ ", FCoords{x, y});
    glue.input().registerTouchEvent(type, {x, y});
}

void renderAudio(void* pGlue, unsigned count, void* buffer) {
    auto& glue = *(Glue*)pGlue;
    glue.instance().audio().mix(reinterpret_cast<float*>(buffer), count);
}
