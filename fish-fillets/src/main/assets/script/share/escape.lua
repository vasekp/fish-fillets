function notify_escape(modelId)
    local model = getModelsTable()[modelId]
    game_killPlan()
    if not small:isAlive() or not big:isAlive() or model:isTalking() then
        return
    end
    if model == small then
        if room.zvykacka and big:isOut() then
            small:planDialog(0, "ob-m-zvykacka")
        else
            small:planDialog(0, "jo-m-"..random(5))
        end
    elseif model == big then
        if random(100) < 15 and small:isOut() then
            big:planDialog(0, "jo-v-4")
        else
            big:planDialog(0, "jo-v-"..random(4))
        end
    else
        if level_isSolved() then
            big:planDialog(0, "jo-v-"..random(5))
            if room.zvykacka then
                small:planDialog(0, "ob-m-zvykacka")
            else
                small:planDialog(0, "jo-m-"..random(5))
            end
        end
    end
end

-- -----------------------------------------------------------------
function escapeLoad()
    file_include("script/share/escape_dialogs.lua")
end

