#ifndef FISH_FILLETS_MODELANIM_H
#define FISH_FILLETS_MODELANIM_H

#include "subsystem/graphics.h"

class ModelAnim {
    std::map<std::string, std::array<std::vector<std::shared_ptr<Image>>, 2>> m_images;
    std::shared_ptr<Image> m_current;

public:
    void add(const std::string& name, int dir, std::shared_ptr<Image> image) {
        m_images[name][dir].push_back(image);
    }

    void set(const std::string& name, int phase) {
        m_current = m_images[name][0][phase];
    }

    const Image& get() const {
        return *m_current;
    }
};

#endif //FISH_FILLETS_MODELANIM_H