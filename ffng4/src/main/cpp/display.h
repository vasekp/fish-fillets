#ifndef FISH_FILLETS_DISPLAY_H
#define FISH_FILLETS_DISPLAY_H

#include "common.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

struct Display {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    std::int32_t width;
    std::int32_t height;

public:
    Display(ANativeWindow* window) {
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

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

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
        surface = eglCreateWindowSurface(display, config, window, nullptr);
        context = eglCreateContext(display, config, nullptr, nullptr);

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
            throw std::runtime_error("Unable to eglMakeCurrent");

        width = eglQuerySurface(display, surface, EGL_WIDTH, &w);
        height = eglQuerySurface(display, surface, EGL_HEIGHT, &h);

//        instance->state.angle = 0;

        auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
        for (auto name : opengl_info) {
            auto info = glGetString(name);
            LOGI("OpenGL Info: %s", info);
        }

        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_DEPTH_TEST);

        LOGD("display: opened %p", display);

//        instance->bg = std::make_unique<Image>(loadImage(SystemFile("images/icon.png", *instance), *instance));
    }

    ~Display() {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);

        LOGD("display: closed %p", display);

        //instance->animating = false;
    }
};

#endif //FISH_FILLETS_DISPLAY_H
