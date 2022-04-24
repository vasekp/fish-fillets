#ifndef HEADER_SELECTLANG_H
#define HEADER_SELECTLANG_H

class File;

#include "VBox.h"
#include "Scripter.h"

/**
 * Menu with flags to select prefered lang.
 */
class SelectLang : public VBox, public Scripter {
    private:
        static const int MAX_WIDTH = 200;
        std::string m_option;
        WiBox *m_activeRow;
    public:
        SelectLang(const std::string &option, const File &datafile);

        void addFlag(const std::string &value, const File &picture);
};

#endif
