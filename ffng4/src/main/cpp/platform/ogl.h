#ifndef FISH_FILLETS_OGL_H
#define FISH_FILLETS_OGL_H

#include "common.h"

#include "EGL/egl.h"
#include "GLES2/gl2.h"

#include "ogl/display.h"
#include "ogl/texture.h"
#include "ogl/shader.h"
#include "ogl/program.h"
#include "ogl/framebuffer.h"

namespace ogl {
    template<typename T>
    [[maybe_unused]] static void rect(T x, T y, T w, T h) {
        float fx = (float)x,
              fy = (float)y,
              fw = (float)w,
              fh = (float)h;
        float coords[4][2] = {
                {fx, fy},
                {fx + fw, fy},
                {fx, fy + fh},
                {fx + fw, fy + fh}};
        glVertexAttribPointer(ogl::Program::aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &coords[0][0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

#endif //FISH_FILLETS_OGL_H