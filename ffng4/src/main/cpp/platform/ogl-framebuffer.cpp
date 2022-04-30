#include "ogl-framebuffer.h"

namespace ogl {

    Framebuffer::Framebuffer(GLuint width, GLuint height) :
            _texture(Texture::empty(width, height)) {
        glGenFramebuffers(1, &name);
        LOGD("framebuffer: gen %d", name);
        glBindFramebuffer(GL_FRAMEBUFFER, name);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _texture.bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Framebuffer::~Framebuffer() {
        LOGD("framebuffer: delete %d", name);
        glDeleteFramebuffers(1, &name);
    }

    void Framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, name);
        glViewport(0, 0, (GLint)width(), (GLint)height());
    }

}