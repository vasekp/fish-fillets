#ifndef HEADER_LABELS_H
#define HEADER_LABELS_H

class File;
class ResDialogPack;

#include "Scripter.h"
#include "StringTool.h"

/**
 * Loads localized labels.
 */
class Labels : public Scripter {
    private:
        mutable ResDialogPack *m_labels;
    public:
        Labels(const File &source);
        virtual ~Labels();

        void addLabel(const std::string &name, const std::string &lang,
                const std::string &text);

        std::string getLabel(const std::string &name) const;
        std::string getFormatedLabel(const std::string &name,
                const StringTool::t_args &args) const;
};

#endif
