#ifndef FISH_FILLETS_GRAPHICS_UTILS_H
#define FISH_FILLETS_GRAPHICS_UTILS_H

namespace GraphicsUtils {

    template<typename T>
    void rect(T x, T y, T w, T h) {
        auto fx = (float)x,
                fy = (float)y,
                fw = (float)w,
                fh = (float)h;
        float coords[4][2] = {
                {fx, fy},
                {fx, fy + fh},
                {fx + fw, fy},
                {fx + fw, fy + fh}};
        glVertexAttribPointer(ogl::Program::aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &coords[0][0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

}

#endif //FISH_FILLETS_GRAPHICS_UTILS_H
