#include "common.h"
#include "assets.h"

static int init_display(struct Instance* instance) {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config = nullptr;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, nullptr, nullptr);
    eglChooseConfig(display, attribs, nullptr,0, &numConfigs);
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    assert(supportedConfigs);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
    assert(numConfigs);

    for (int i = 0; i < numConfigs; i++) {
        auto& candidate = supportedConfigs[i];
        EGLint r, g, b;
        if (eglGetConfigAttrib(display, candidate, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(display, candidate, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, candidate, EGL_BLUE_SIZE, &b) &&
            r == 8 && g == 8 && b == 8) {

            config = candidate;
            break;
        }
    }

    if (config == nullptr) {
        LOGW("No best fit found, using first supported config");
        config = supportedConfigs[0];
    }

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, instance->app->window, nullptr);
    context = eglCreateContext(display, config, nullptr, nullptr);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGE("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    instance->display = display;
    instance->context = context;
    instance->surface = surface;
    instance->width = w;
    instance->height = h;
    instance->state.angle = 0;

    auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
    for (auto name : opengl_info) {
        auto info = glGetString(name);
        LOGI("OpenGL Info: %s", info);
    }

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);

    instance->bg = std::make_unique<Image>(loadImage(SystemFile("images/icon.png", *instance), *instance));

    return 0;
}

static void draw_frame(struct Instance* instance) {
    if (instance->display == nullptr)
        return;

    // Just fill the screen with a color.
    glClearColor(instance->state.x/(float)instance->width, instance->state.angle,
                 instance->state.y/(float)instance->height, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(instance->display, instance->surface);
}

static void term_display(struct Instance* instance) {
    if (instance->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(instance->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (instance->context != EGL_NO_CONTEXT) {
            eglDestroyContext(instance->display, instance->context);
        }
        if (instance->surface != EGL_NO_SURFACE) {
            eglDestroySurface(instance->display, instance->surface);
        }
        eglTerminate(instance->display);
    }
    instance->animating = false;
    instance->display = EGL_NO_DISPLAY;
    instance->context = EGL_NO_CONTEXT;
    instance->surface = EGL_NO_SURFACE;
}

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto* instance = (struct Instance*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        instance->animating = true;
        instance->state.x = AMotionEvent_getX(event, 0);
        instance->state.y = AMotionEvent_getY(event, 0);
        return 1;
    }
    return 0;
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    auto* instance = (struct Instance*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            instance->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)instance->app->savedState) = instance->state;
            instance->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            if (instance->app->window != nullptr) {
                init_display(instance);
                draw_frame(instance);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            term_display(instance);
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            instance->animating = false;
            draw_frame(instance);
            break;
        default:
            break;
    }
}

void android_main(struct android_app* app) {
    struct Instance instance{};

    memset(&instance, 0, sizeof(instance));
    app->userData = &instance;
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    instance.app = app;

    app->activity->vm->AttachCurrentThread(&instance.jni, nullptr);

    if (app->savedState != nullptr)
        instance.state = *(struct saved_state*)app->savedState;

    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;
        bool quit = false;

        while ((ident=ALooper_pollAll(instance.animating ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0) {
            if (source != nullptr)
                source->process(app, source);
            if (ident == LOOPER_ID_USER) {
                // sensors...
            }
            if (app->destroyRequested != 0) {
                term_display(&instance);
                quit = true;
            }
        }

        if(quit)
            break;

        if (instance.animating) {
            instance.state.angle += .01f;
            if (instance.state.angle > 1)
                instance.state.angle = 0;
            draw_frame(&instance);
        }

        app->activity->vm->DetachCurrentThread();
    }
}