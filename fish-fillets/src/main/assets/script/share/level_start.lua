-- -----------------------------------------------------------------
-- This script contains script_update() function,
-- which is called every game cycle from the game.
--
-- NOTE: this script contains code structures to support
-- original level design (e.g. afaze, X, Y, dir)
-- -----------------------------------------------------------------

file_include("script/share/boredom.lua")
file_include("script/share/blackjokes.lua")
file_include("script/share/bubbles.lua")
file_include("script/share/escape.lua")


-- -----------------------------------------------------------------
-- Init function
-- -----------------------------------------------------------------
function initModels()
    -- Set starting values for all models
    -- Run this function in you init
    local models = getModelsTable()
    for key, model in pairs(models) do
        model.afaze = 0
        model.X, model.Y = model:getLoc()
        model.XStart, model.YStart = model:getLoc()
        model.dir = dir_no
        model.updateAnim = function(self)
            self:setAnim("default", self.afaze)
        end
        model.anim = ""
        resetanim(model)
    end

    escapeLoad()
    stdBoredomLoad()
    stdBlackJokeLoad()
    stdBublesLoad()
    loadColors()
end


-- -----------------------------------------------------------------
-- Run functions
-- -----------------------------------------------------------------
local wasRestart = true
local function updateModels()
    if not wasRestart and not level_isNewRound() then
        return
    end
    wasRestart = false

    -- update .X, .Y for all models
    local models = getModelsTable()
    for key, model in pairs(models) do
        model.X, model.Y = model:getLoc()
        model.lookLeft = model:isLeft()

        local action = model:getAction()
        if "move_up" == action then
            model.dir = dir_up
        elseif "move_down" == action then
            model.dir = dir_down
        elseif "move_left" == action then
            model.dir = dir_left
        elseif "move_right" == action then
            model.dir = dir_right
        else
            model.dir = dir_no
        end
    end
end


-- -----------------------------------------------------------------
function script_update()
    -- this function is called after every game cycle
    animateUnits()

    updateModels()
    prog_update()

    stdBubles()
    stdBoredom()
    stdBlackJoke()
end
