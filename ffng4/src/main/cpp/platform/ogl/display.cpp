#include "platform/ogl.h"

namespace ogl {

    Display::Display(ANativeWindow *window) {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(display, nullptr, nullptr);

        EGLConfig config = [&]() {
            const EGLint attribs[] = {
                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL_BLUE_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_RED_SIZE, 8,
                    EGL_NONE
            };

            EGLint numConfigs;

            eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);
            std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
            if(!supportedConfigs)
                ::error("supportedConfigs failed");
            eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
            if(!numConfigs)
                ::error("eglChooseConfig failed");

            for (int i = 0; i < numConfigs; i++) {
                auto &config = supportedConfigs[i];
                EGLint r, g, b;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &r) &&
                    eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &g) &&
                    eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &b) &&
                    r == 8 && g == 8 && b == 8) {

                    return config;
                }
            }

            LOGW("No best fit found, using first supported config");
            return supportedConfigs[0];
        }();

        context = [&]() -> EGLContext {
            EGLint attribs[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
            };
            return eglCreateContext(display, config, nullptr, attribs);
        }();

        surface = eglCreateWindowSurface(display, config, window, nullptr);

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
            ::error("eglMakeCurrent failed");

        _width = _height = 0;
        eglQuerySurface(display, surface, EGL_WIDTH, &_width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &_height);

        auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
        for (auto name : opengl_info) {
            auto info = glGetString(name);
            LOGV("OpenGL Info: %s", info);
        }

        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnableVertexAttribArray(0);

        LOGD("display: opened %p [%d x %d]", display, _width, _height);
    }

    Display::~Display() {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);

        LOGD("display: closed %p", display);
    }

    void Display::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width(), height());
    }

    void Display::swap() const {
        eglSwapBuffers(display, surface);
    }

}