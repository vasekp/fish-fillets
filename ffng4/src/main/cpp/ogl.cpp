#include "ogl.h"

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

        width = height = 0;
        eglQuerySurface(display, surface, EGL_WIDTH, &width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &height);

        auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
        for (auto name : opengl_info) {
            auto info = glGetString(name);
            LOGI("OpenGL Info: %s", info);
        }

        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        LOGD("display: opened %p [%d x %d]", display, width, height);
    }

    Display::~Display() {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);

        LOGD("display: closed %p", display);
    }

    void Display::swap() const {
        eglSwapBuffers(display, surface);
    }

    Shader::Shader(GLenum type, const std::string& code) {
        handle = glCreateShader(type);
        if(!handle)
            ::error("glCreateShader failed");

        const auto source = code.c_str();
        glShaderSource(handle, 1, &source, nullptr);
        glCompileShader(handle);

        int status;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

        if(status == GL_FALSE) {
            int loglen, actlen;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetShaderInfoLog(handle, loglen, &actlen, error.data());
            glDeleteShader(handle);
            ::error("glCompileShader failed", "%s", error.c_str());
        }
    }

    Shader::~Shader() {
        glDeleteShader(handle);
    }

    Program::Program(const Shader& vertexShader, const Shader& fragmentShader) {
        handle = glCreateProgram();
        if(!handle)
            ::error("glCreateProgram failed");

        glAttachShader(handle, vertexShader);
        glAttachShader(handle, fragmentShader);
        glLinkProgram(handle);

        int status;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);

        if(status == GL_FALSE) {
            int loglen, actlen;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetProgramInfoLog(handle, loglen, &actlen, error.data());
            glDeleteProgram(handle);
            ::error("glLinkProgram failed", "%s", error.c_str());
        }
    }

    Program::~Program() {
        glDeleteProgram(handle);
    }

}