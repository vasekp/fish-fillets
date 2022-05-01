#include "game/screen.h"

void GameScreen::setBackground(const std::string &filename) {
    m_images.insert_or_assign("background", Image(filename));
}

void GameScreen::setMusic(const std::string& filename) {
    m_music = std::make_shared<AudioSource>(m_instance->audio->loadAudio(filename));
}

void GameScreen::reloadImages() {
    for(auto& [_, image] : m_images)
        image.reload(m_instance);
}

void GameScreen::start() {
    LOGD("screen: start");
    m_music->rewind();
    load();
}

void GameScreen::load() {
    LOGD("screen: load");
    reloadImages();

    try {
        auto& bgImage = m_images.at("background");
        m_instance->graphics->setCanvasSize(bgImage.width(), bgImage.height());
    } catch(std::out_of_range& e) {
        ::error("Level has no background set.");
    }

    m_instance->audio->clear();
    m_instance->audio->addSource(m_music);
}

void GameScreen::drawFrame() {
    if (!m_instance->graphics)
        return;

    const auto* display = m_instance->graphics->display();
    const auto* canvas = m_instance->graphics->canvas();

    canvas->bind();
    own_draw();

    display->bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    auto& program = m_instance->graphics->shaders()->fill;
    glUseProgram(program);
    auto fsw = (float)canvas->width();
    auto fsh = (float)canvas->height();
    auto fdw = (float)display->width();
    auto fdh = (float)display->height();
    glUniform1i(program.uniform("uScreenTexture"), Shaders::texCanvas_shader);
    glUniform2f(program.uniform("uDisplaySize"), fdw, fdh);
    glUniform2f(program.uniform("uCanvasSize"), fsw, fsh);
    glUniform2f(program.uniform("uTextureSize"), (float) canvas->texWidth(), (float) canvas->texHeight());

    float scale = std::min(fdw / fsw, fdh / fsh);
    glViewport((int)(fdw - scale * fsw) / 2,
               (int)(fdh - scale * fsh) / 2,
               (int)(scale * fsw), (int)(scale * fsh));

    ogl::rect(-1.f, -1.f, 2.f, 2.f);

    display->swap();
}