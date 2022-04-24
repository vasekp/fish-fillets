/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "ModelList.h"

#include "View.h"
#include "Landslip.h"

//-----------------------------------------------------------------
/**
 * Create new wrapper.
 * @param models wrapped models.
 */
ModelList::ModelList(const Cube::t_models_owning *models)
{
    m_models = models;
}
//-----------------------------------------------------------------
void
ModelList::drawOn(View *view) const
{
    for(const auto& model : *m_models) {
        view->drawModel(model.get());
    }
}
//-----------------------------------------------------------------
/**
 * Stone all models on fixed pad.
 * @return true when new model was stoned
 */
bool
ModelList::stoneOn(Landslip *slip) const
{
    bool change = false;
    for(const auto& model : *m_models) {
        if (slip->stoneModel(model.get())) {
            change = true;
        }
    }
    return change;
}
//-----------------------------------------------------------------
/**
 * Let all not stoned models to fall.
 * @return true when something is falling
 */
bool
ModelList::fallOn(Landslip *slip) const
{
    bool falling = false;
    for(const auto& model : *m_models) {
        falling |= slip->fallModel(model.get());
    }
    return falling;
}

