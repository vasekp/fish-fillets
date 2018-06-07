
-- -----------------------------------------------------------------
-- NOTE: uses 'small' and 'big' fish names
local function playShout(unit)
    if isReady(unit) and not unit:isTalking() and level_isNewRound() then
        if unit:getState() == "goout" or level_isSolved() then
            if unit == small then
                if room.zvykacka and (big:isOut() or level_isSolved()) then
                    --NOTE: chewing gum bore joke
                    small:talk("ob-m-zvykacka")
                else
                    unit:talk("sp-shout_small_0"..random(5))
                end
            elseif unit == big then
                if random(100) < 15 and (small:isOut() or level_isSolved()) then
                    unit:talk("sp-shout_big_04")
                else
                    unit:talk("sp-shout_big_0"..random(4))
                end
            end
            game_killPlan()
        end
    end
end

-- -----------------------------------------------------------------
function borderShout()
    for index, unit in pairs(getUnitTable()) do
        playShout(unit)
    end
end

-- -----------------------------------------------------------------
function borderShoutLoad()
    --TODO: add 'en' sound
    dialog_addDialog("sp-shout_small_00", "en", "")
    dialog_addDialog("sp-shout_small_01", "en", "")
    dialog_addDialog("sp-shout_small_02", "en", "")
    dialog_addDialog("sp-shout_small_03", "en", "")
    dialog_addDialog("sp-shout_small_04", "en", "")

    dialog_addDialog("sp-shout_big_00", "en", "")
    dialog_addDialog("sp-shout_big_01", "en", "")
    dialog_addDialog("sp-shout_big_02", "en", "")
    dialog_addDialog("sp-shout_big_03", "en", "")
    dialog_addDialog("sp-shout_big_04", "en", "")


    dialog_addDialog("sp-shout_small_00", "cs", "sound/share/border/cs/sp-shout_small_00.ogg")
    dialog_addDialog("sp-shout_small_01", "cs", "sound/share/border/cs/sp-shout_small_01.ogg")
    dialog_addDialog("sp-shout_small_02", "cs", "sound/share/border/cs/sp-shout_small_02.ogg")
    dialog_addDialog("sp-shout_small_03", "cs", "sound/share/border/cs/sp-shout_small_03.ogg")
    dialog_addDialog("sp-shout_small_04", "cs", "sound/share/border/cs/sp-shout_small_04.ogg")

    dialog_addDialog("sp-shout_big_00", "cs", "sound/share/border/cs/sp-shout_big_00.ogg")
    dialog_addDialog("sp-shout_big_01", "cs", "sound/share/border/cs/sp-shout_big_01.ogg")
    dialog_addDialog("sp-shout_big_02", "cs", "sound/share/border/cs/sp-shout_big_02.ogg")
    dialog_addDialog("sp-shout_big_03", "cs", "sound/share/border/cs/sp-shout_big_03.ogg")
    dialog_addDialog("sp-shout_big_04", "cs", "sound/share/border/cs/sp-shout_big_04.ogg")
end

