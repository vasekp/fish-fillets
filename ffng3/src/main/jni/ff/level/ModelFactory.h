#ifndef HEADER_MODELFACTORY_H
#define HEADER_MODELFACTORY_H

class V2;
class Unit;

#include "common.h"
#include "Cube.h"
#include "ControlSym.h"
#include "Dir.h"

/**
 * Knows how to create new models.
 */
class ModelFactory {
    private:
        static void createParams(const std::string &kind,
                Cube::eWeight *out_weight, Cube::eWeight *out_power,
                bool *out_alive);
        static ControlSym parseExtraControlSym(const std::string &kind);
        static std::unique_ptr<Cube> createOutputItem(const std::string &kind, const V2 &loc,
                const std::string &shape);
    public:
        static std::unique_ptr<Cube> createModel(const std::string &kind, const V2 &loc,
                const std::string &shape);
        static std::unique_ptr<Unit> createUnit(const std::string &kind);
        static std::unique_ptr<Cube> createBorder();
};

#endif
