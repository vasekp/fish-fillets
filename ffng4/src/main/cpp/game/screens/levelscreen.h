#ifndef FISH_FILLETS_LEVELSCREEN_H
#define FISH_FILLETS_LEVELSCREEN_H

#include "screen.h"
#include "levelinput.h"
#include "game/structure/levelrecord.h"
#include "game/logic/level.h"
#include "subsystem/audio.h"
#include "game/logic/timer.h"
#include "game/graphics/subtitles.h"

class LevelScreen : public GameScreen {
    Level m_level;
    LevelInput m_input;
    std::multimap<std::string, AudioData::Ref> m_sounds;
    AudioSource::Ref m_music;
    std::array<float, 3> m_waves;
    Timer m_timer;
    Subtitles m_subs;
    std::optional<Image> m_display;
    bool m_fullLoad;
    bool m_quit;

    std::unique_ptr<TextureTarget> m_mirrorTarget;
    FCoords m_shift;

public:
    LevelScreen(Instance&, LevelRecord&);

    IInput& input() override { return m_input; }
    Subtitles& subs() { return m_subs; }
    Timer& timer() { return m_timer; }

    void display(const std::string& filename);
    void restore();
    void setWaves(float amplitude, float period, float speed);
    AudioData::Ref addSound(const std::string& name, const std::string& filename, bool single = false);
    using GameScreen::addImage;
    using GameScreen::replaceImage;
    void addSubtitle(const std::string& text, const std::string& color);
    void playMusic(const std::string& filename);
    void stopMusic();
    void killSounds();
    void setShift(FCoords shift);
    FCoords shift() override;
    const auto& sounds() const { return m_sounds; }
    void exit();

    constexpr static int size_unit = 15;

private:
    void own_start() override;
    void own_refresh() override;
    void own_pause() override;
    void own_resume() override;
    void own_draw(const DrawTarget& target, float dt) override;
    void own_drawOverlays(const DrawTarget &target, float dTime, float absTime) override;
    bool own_mouse(unsigned int x, unsigned int y) override;
    bool own_key(Key key) override;
    void own_setsize() override;

    void leave();

    std::unique_ptr<TextureTarget> makeMirrorTarget(const Model& model);
};

#endif //FISH_FILLETS_LEVELSCREEN_H