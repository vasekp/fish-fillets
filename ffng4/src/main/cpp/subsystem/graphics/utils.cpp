#include "subsystem/graphics.h"

namespace GraphicsUtils {

    template<typename T>
    void rect(T x, T y, T w, T h) {
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

    template void rect(float, float, float, float);

    template void rect(unsigned, unsigned, unsigned, unsigned);

}