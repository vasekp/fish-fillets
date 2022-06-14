#ifndef FISH_FILLETS_FILES_H
#define FISH_FILLETS_FILES_H

#include "instance.h"
#include "files/ifile.h"
#include "platform/files.h"

class Files {
    Instance& m_instance;

public:
    Files(Instance& instance) : m_instance(instance) { }

    SystemFile system(const std::string& path) const;
    UserFile user(const std::string& path) const;
};

#endif //FISH_FILLETS_FILES_H
