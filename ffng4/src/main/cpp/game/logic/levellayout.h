#ifndef FISH_FILLETS_LEVELLAYOUT_H
#define FISH_FILLETS_LEVELLAYOUT_H

#include "model.h"
#include "subsystem/key.h"

class Level;

class LevelLayout {
    [[maybe_unused]] Level& m_level;
    [[maybe_unused]] int m_width;
    [[maybe_unused]] int m_height;

    std::vector<std::unique_ptr<Model>> m_models;
    std::map<int, std::size_t> m_virtModels;
    Model* m_small;
    Model* m_big;
    Model* m_curFish;
    std::map<const Model*, Model::SupportType> m_support;
    std::queue<Key> m_keyQueue;
    bool m_moving;
    bool m_ready;

public:
    LevelLayout(Level& level, int width, int height);

    void prepare();
    auto& models() { return m_models; }
    int addModel(const std::string& type, int x, int y, const std::string& shape);
    Model& getModel(int index);

    void moveFish(ICoords d);
    void switchFish();

    void keyInput(Key key);
    void update(float dt);

private:
    bool animate(float dt);
    void reeval();
    void processKey(Key key);
    void clearQueue();
    std::set<Model*> obstacles(const Model& unit, ICoords d);
    void buildSupportMap();
};

#endif //FISH_FILLETS_LEVELLAYOUT_H