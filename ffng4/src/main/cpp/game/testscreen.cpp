#include "testscreen.h"

TestScreen::TestScreen(Instance* instance, const char* a, const char* b) : GameScreen(instance) {
    setBackground(a);
    setMusic(b);
}

void TestScreen::own_draw() {
    auto& bgImage = m_images.at("background");
    const auto& canvas = m_instance->graphics->canvas();

    bgImage.texture().bind();

    auto& program = m_instance->graphics->shaders()->copy;
    glUseProgram(program);
    glUniform1i(program.uniform("uTexture"), Shaders::texImage_shader);
    glUniform2f(program.uniform("uSrcSize"), (float) bgImage.width(), (float) bgImage.height());
    glUniform2f(program.uniform("uDstSize"), (float) canvas->width(), (float) canvas->height());
    glUniform2f(program.uniform("uSrcOffset"), 0.f, 0.f);
    glUniform2f(program.uniform("uDstOffset"), 0.f, 0.f);

    ogl::rect(0u, 0u, bgImage.width(), bgImage.height());
}