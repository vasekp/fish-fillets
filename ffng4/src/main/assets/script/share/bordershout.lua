function notify_escape(modelId)
    local model = getModelsTable()[modelId]
    game_killPlan()
    if not small:isAlive() or not big:isAlive() or model:isTalking() then
        return
    end
    if model == small then
        if room.zvykacka and big:isOut() then
            small:talk("ob-m-zvykacka")
        else
            small:talk("sp-shout_small_0"..random(5))
        end
    elseif model == big then
        if room.zvykacka and small.isOut() then
            small:talk("ob-m-zvykacka")
        elseif random(100) < 15 and small:isOut() then
            big:talk("sp-shout_big_04")
        else
            big:talk("sp-shout_big_0"..random(4))
        end
    else
        model:setGoal("goal_none")
        if level_isSolved() then
            if not small:isTalking() then
                small:talk("sp-shout_small_0"..random(5))
            end
            if not big:isTalking() then
                big:talk("sp-shout_big_0"..random(5))
            end
            small:setGoal("goal_escape");
            big:setGoal("goal_escape");
        end
    end
    planTimeAction(30, function()
        if level_isSolved() then
            level_action_saveQuit()
        end
    end)
end

-- -----------------------------------------------------------------
function borderShoutLoad()
    dialogLoad("script/share/shout_", "sound/share/border/")
end

