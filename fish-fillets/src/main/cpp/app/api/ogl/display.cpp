#include "api/ogl.h"

namespace ogl {

    template<typename... NativeArgs>
    Display::Display(const NativeArgs& ... nativeArgs) : m_ref(std::make_shared<int>(1)) {
        m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(m_display, nullptr, nullptr);

        EGLConfig config = [&]() {
            const EGLint attribs[] = {
                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL_BLUE_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_RED_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,
                    EGL_NONE
            };

            EGLint numConfigs;

            eglChooseConfig(m_display, attribs, nullptr, 0, &numConfigs);
            auto supportedConfigs = std::make_unique<EGLConfig[]>(numConfigs);
            eglChooseConfig(m_display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
            if(!numConfigs)
                Log::fatal("eglChooseConfig failed");

            for (int i = 0; i < numConfigs; i++) {
                auto& config = supportedConfigs[i];
                EGLint r, g, b, a;
                if(eglGetConfigAttrib(m_display, config, EGL_RED_SIZE, &r) &&
                        eglGetConfigAttrib(m_display, config, EGL_GREEN_SIZE, &g) &&
                        eglGetConfigAttrib(m_display, config, EGL_BLUE_SIZE, &b) &&
                        eglGetConfigAttrib(m_display, config, EGL_ALPHA_SIZE, &a) &&
                        r == 8 && g == 8 && b == 8 && a == 8) {
                    return config;
                }
            }

            Log::warn("No best fit found, using first supported config");
            return supportedConfigs[0];
        }();

        m_context = [&]() -> EGLContext {
            EGLint attribs[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
            };
            return eglCreateContext(m_display, config, nullptr, attribs);
        }();

        m_surface = eglCreateWindowSurface(m_display, config, nativeArgs..., nullptr);

        if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE)
            Log::fatal("eglMakeCurrent failed");

        {
            std::int32_t width;
            std::int32_t height;
            eglQuerySurface(m_display, m_surface, EGL_WIDTH, &width);
            eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &height);
            m_origin = {};
            m_size = {width, height};

            Log::debug<Log::graphics>("display: opened ", (void*)m_display, " ", m_size);
        }
        auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
        for (auto name : opengl_info) {
            auto info = glGetString(name);
            Log::verbose<Log::graphics>("OpenGL Info: ", info);
        }

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        glEnableVertexAttribArray(0);

        glClearColor(0, 0, 0, 1);
        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    }

    template Display::Display(const NativeWindowType&);

    Display& Display::operator=(Display&& other) {
        m_display = other.m_display;
        m_surface = other.m_surface;
        m_context = other.m_context;
        m_origin = other.m_origin;
        m_size = other.m_size;
        m_ref = std::move(other.m_ref);
        other.m_display = EGL_NO_DISPLAY;
        other.m_surface = EGL_NO_SURFACE;
        other.m_context = EGL_NO_CONTEXT;
        return *this;
    }

    Display::Display(Display&& other) {
        *this = std::move(other);
    }

    Display::~Display() {
        if(m_display == EGL_NO_DISPLAY)
            return;

        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_display, m_context);
        eglDestroySurface(m_display, m_surface);
        eglTerminate(m_display);

        Log::debug<Log::graphics>("display: closed ", (void*)m_display);
    }

    void Display::setViewport(ICoords origin, ICoords size) {
        m_origin = origin;
        m_size = size;
    }

    void Display::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(m_origin.x, m_origin.y, m_size.x, m_size.y);
    }

    void Display::swap() const {
        eglSwapBuffers(m_display, m_surface);
    }

}
