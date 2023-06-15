#ifndef FISH_FILLETS_LEVELSCREEN_H
#define FISH_FILLETS_LEVELSCREEN_H

#include "screen.h"
#include "levelinput.h"
#include "game/structure/levelrecord.h"
#include "game/logic/level.h"
#include "subsystem/audio.h"
#include "game/graphics/subtitles.h"
#include "game/graphics/zxeffect.h"

class LevelScreen : public GameScreen {
    Level m_level;
    LevelInput m_input;
    std::multimap<std::string, AudioData::Ref> m_sounds;
    AudioSource::Ref m_music;
    std::array<float, 3> m_waves;
    FCoords m_winSize;
    Subtitles m_subs;
    std::optional<PNGImage> m_display;
    bool m_quit;

    std::unique_ptr<TextureTarget> m_mirrorTarget;
    std::unique_ptr<ZXEffect> m_zxEffect;
    float m_flashAlpha;

public:
    LevelScreen(Instance&, LevelRecord&);

    Level& level() { return m_level; }
    IInputSink& input() override { return m_input; }
    Subtitles& subs() { return m_subs; }

    void display(const std::string& filename);
    void restore();
    void exit();

    bool longPress(FCoords coords);
    bool doubleTap(FCoords coords);

    void setWaves(float amplitude, float period, float speed);
    AudioData::Ref addSound(const std::string& name, const std::string& filename, bool single = false);
    using GameScreen::addImage;
    using GameScreen::replaceImage;
    void addSubtitle(const std::string& text, const std::string& color);
    void playMusic(const std::string& filename);
    void stopMusic();
    void playSound(const std::string& filename, float volume = 1.f);
    void killSounds();
    void saveEffect();
    void setShift(FCoords shift);

    constexpr static int size_unit = 15;

private:
    void own_start() override;
    void own_resize() override;
    void own_pause() override;
    void own_resume() override;
    void own_draw(const DrawTarget& target, float dt) override;
    void own_drawOverlays(const DrawTarget &target, float dTime, float absTime) override;
    bool own_key(Key key) override;

    void leave();

    std::unique_ptr<TextureTarget> makeMirrorTarget(const Model& model);

    constexpr static float flashInit = 0.5f;
    constexpr static float flashDecay = 1.5f;
};

#endif //FISH_FILLETS_LEVELSCREEN_H
