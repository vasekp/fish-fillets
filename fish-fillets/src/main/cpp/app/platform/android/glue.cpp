#include "glue.h"
#include "log.h"
#include "ainstance.h"
#include "subsystem/rng.h"
#include "subsystem/graphics/graphicsbackend.h"
#include "api/jni.h"

Glue::Glue(JNIEnv* env, jobject activity, jobject jAssets, std::string&& userPath) {
    JavaVM* vm;
    env->GetJavaVM(&vm);
    m_mainEnv = env;
    m_activity = env->NewGlobalRef(activity);
    m_assets = env->NewGlobalRef(jAssets);
    m_msgQueued = false;
    m_running = false;
    m_thread = std::thread{[&]() { worker(vm, std::move(userPath)); }};
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [this]() { return m_running; });
    Log::info<Log::lifecycle>("thread running (main)");
}

Glue::~Glue() {
    submitMessage(QuitMessage{});
    m_thread.join();
    Log::info<Log::lifecycle>("thread quitted (main)");
    m_mainEnv->DeleteGlobalRef(m_activity);
    m_mainEnv->DeleteGlobalRef(m_assets);
}

void Glue::submitMessage(const Message& message) {
    m_message = message;
    m_msgQueued.store(true, std::memory_order::release);
    m_cond.notify_one();
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [this]() { return !m_msgQueued.load(std::memory_order::relaxed); });
}

void Glue::closeActivity() {
    auto& env = m_instance->jni;
    jclass clazz = env->GetObjectClass(m_activity);
    jmethodID method = env->GetMethodID(clazz, "finish", "()V");
    env->CallVoidMethod(m_activity, method);
}

void Glue::worker(JavaVM* vm, std::string&& userPath) {
    jni::Env localEnv{vm, m_activity};
    AndroidInstance instance{*this, localEnv, m_assets, std::move(userPath)};
    m_instance = &instance;

    m_running = true;
    Log::info<Log::lifecycle>("thread running (worker)");
    m_cond.notify_one();

    instance.oboe().open();
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
    instance.oboe().close();
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

AndroidInput& Glue::input() {
    return m_instance->inputSource();
}

std::map<jint, std::unique_ptr<Glue>> global_map;

extern "C"
JNIEXPORT jint JNICALL
Java_cz_absolutno_fillets_MainActivity_startApp(JNIEnv* env, jobject activity, jobject jAMgr, jstring jUserPath) {
    RNG rng;
    jint index;
    do {
        index = (jint)rng.randomIndex(std::numeric_limits<jint>::max());
    } while(global_map.contains(index));

    const char* utf = env->GetStringUTFChars(jUserPath, nullptr);
    std::string userPath{utf};
    env->ReleaseStringUTFChars(jUserPath, utf);

    auto glue = std::make_unique<Glue>(env, activity, jAMgr, std::move(userPath));
    global_map.insert_or_assign(index, std::move(glue));

    Log::verbose<Log::lifecycle>("onCreate (C++): index = ", index, " count = ", global_map.size());

    return index;
}

extern "C"
JNIEXPORT void JNICALL
Java_cz_absolutno_fillets_MainActivity_quitApp(JNIEnv* env, jobject, jint handle) {
    Log::verbose<Log::lifecycle>("onDestroy (C++), handle = ", handle);
    global_map.erase(handle);
}

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

extern "C"
JNIEXPORT jboolean JNICALL
Java_cz_absolutno_fillets_MainActivity_pointerAction(JNIEnv* env, jobject thiz, jint handle, jint action, jfloat x, jfloat y) {
    Log::verbose<Log::lifecycle>("pointer event (C++), action = ", action, " x = ", x, " y = ", y);
    auto& glue = *global_map.at(handle);
    glue.input().registerTouchEvent(action, {x, y});
    return true;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cz_absolutno_fillets_MainActivity_keyAction(JNIEnv* env, jobject thiz, jint handle, jint action, jint keyCode) {
    Log::verbose<Log::lifecycle>("key event (C++), action = ", action, " code = ", keyCode);
    auto& glue = *global_map.at(handle);
    return glue.input().registerKeyEvent(action, keyCode);
}