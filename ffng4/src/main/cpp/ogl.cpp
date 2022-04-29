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

        _width = _height = 0;
        eglQuerySurface(display, surface, EGL_WIDTH, &_width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &_height);

        auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
        for (auto name : opengl_info) {
            auto info = glGetString(name);
            LOGI("OpenGL Info: %s", info);
        }

        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

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

    Texture::Texture(GLuint width, GLuint height) : _width(width), _height(height) {
        glGenTextures(1, &name);
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    Texture::Texture(Texture&& other) noexcept :
        _width(other._width), _height(other._height), name(other.name)
    {
        other.name = 0;
    }

    Texture Texture::fromImageData(GLuint width, GLuint height, std::size_t stride, void *data) {
        assert(stride == 4 * width); // GLES does not support GL_UNPACK_ROW_LENGTH
        Texture ret{width, height};
        ret.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        return ret;
    }

    Texture Texture::empty(GLuint width, GLuint height) {
        Texture ret{width, height};
        ret.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        return ret;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        _width = other._width;
        _height = other._height;
        std::swap(name, other.name);
        return *this;
    }

    Texture::~Texture() {
        if(name)
            glDeleteTextures(1, &name);
    }

    void Texture::bind() const {
        glBindTexture(GL_TEXTURE_2D, name);
    };

    Shader::Shader(GLenum type, const std::string& code) {
        name = glCreateShader(type);
        if(!name)
            ::error("glCreateShader failed");

        const auto source = code.c_str();
        glShaderSource(name, 1, &source, nullptr);
        glCompileShader(name);

        int status;
        glGetShaderiv(name, GL_COMPILE_STATUS, &status);

        if(status == GL_FALSE) {
            int loglen, actlen;
            glGetShaderiv(name, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetShaderInfoLog(name, loglen, &actlen, error.data());
            glDeleteShader(name);
            ::error("glCompileShader failed", "%s", error.c_str());
        }
    }

    Shader::~Shader() {
        glDeleteShader(name);
    }

    Program::Program(const Shader& vertexShader, const Shader& fragmentShader) {
        name = glCreateProgram();
        if(!name)
            ::error("glCreateProgram failed");

        glAttachShader(name, vertexShader);
        glAttachShader(name, fragmentShader);
        glLinkProgram(name);

        int status;
        glGetProgramiv(name, GL_LINK_STATUS, &status);

        if(status == GL_FALSE) {
            int loglen, actlen;
            glGetProgramiv(name, GL_INFO_LOG_LENGTH, &loglen);
            std::string error(loglen, '\0');
            glGetProgramInfoLog(name, loglen, &actlen, error.data());
            glDeleteProgram(name);
            ::error("glLinkProgram failed", "%s", error.c_str());
        }
    }

    Program::~Program() {
        glDeleteProgram(name);
    }

    Framebuffer::Framebuffer(GLuint maxWidth, GLuint maxHeight) :
        _texture(Texture::empty(maxWidth, maxHeight)),
        _width(maxWidth),
        _height(maxHeight)
    {
        glGenFramebuffers(1, &name);
        glBindFramebuffer(GL_FRAMEBUFFER, name);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &name);
    }

    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, name);
        glViewport(0, 0, width(), height());
    }

    void Framebuffer::resize(GLuint width, GLuint height) {
        assert(width <= _texture.width() && height <= _texture.height());
        _width = width;
        _height = height;
    }
}