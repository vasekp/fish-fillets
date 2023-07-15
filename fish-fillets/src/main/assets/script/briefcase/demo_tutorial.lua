local function syncTalk()
    planTimeAction(0, function() end)
end

local function syncMoves()
    game_planAction(function() return level_isReady() end)
end

local function planMoveAction(delay, action) -- TODO precondition, postcondition
    local waitTime = delay
    game_planAction(function()
        if waitTime > 0 then
            waitTime = waitTime - 1
            return false
        else
            return action()
        end
    end)
end

local function planPushingAnim(delay)
    syncMoves()
    local time = delay
    planMoveAction(0, function()
        small:useSpecialAnim("head_pushing", 0)
        time = time - 1
        return time <= 0
    end)
end

file_include("script/"..codename.."/tutorial_dialogs.lua")

demo_enter()
addm(0, "help1") -- Teď na nic nesahej
planMoveAction(3, function() return level_action_showMoves("llll") end)
planMoveAction(0, function() return small:goTo(17, 25) end)
planMoveAction(3, function() return level_action_showMoves("LLL") end)
planMoveAction(0, function() return big:goTo(17, 24) end)
syncMoves()
addv(0, "help2") -- Než vstoupíme do dílny
planTimeAction(0, function() game_flashButton("save") end)
planTimeAction(5, function() level_action_save() end)

planMoveAction(6, function() return small:goTo(7, 17) end)
planMoveAction(0, function() return level_action_showMoves("r") end)
planMoveAction(3, function() return level_action_showMoves("LLLL") end)
addm(0, "help3") -- co nám ubližuje
addv(0, "help4") -- Já budu dělat figuranta.
addm(0, "help5") -- neměli házet předměty
syncTalk()
planMoveAction(3, function() return level_action_showMoves("r") end)
planMoveAction(3, function() return level_action_showMoves("r") end) -- velká zemře
planMoveAction(13, function() return small:goTo(7, 25) end)
planMoveAction(3, function() return level_action_showMoves("uuuu") end)
addm(0, "help6") -- nesmíme podklesnout
syncTalk()
planMoveAction(0, function() return level_action_showMoves("d") end) -- malá zemře
syncMoves()
planTimeAction(20, function() level_action_restart() end)

addv(10, "help7") -- Nyní začínáme znovu
planTimeAction(0, function() game_flashButton("load") end)
planTimeAction(5, function() level_action_load() end)

addv(8, "help8") -- jsme na místě, kde jsme hru ukládali
addm(0, "help9") -- další věc... posunovat si předmět po hřbetě
planMoveAction(0, function() return big:goTo(12, 30) end)
planMoveAction(8, function() return level_action_showMoves("UU") end)
syncTalk()
planMoveAction(8, function() return level_action_showMoves("ll") end) -- velká zemře
syncMoves()
addm(5, "help10") -- Některé předměty mají takový tvar
planMoveAction(0, function() return small:goTo(6, 25) end)
syncTalk()
planMoveAction(5, function() return level_action_showMoves("uuuuuuul") end) -- malá zemře
syncMoves()
planTimeAction(20, function() level_action_restart() end)

addv(10, "help11") -- Znovu nahrajeme
planTimeAction(0, function() game_flashButton("load") end)
planTimeAction(5, function() level_action_load() end)

planMoveAction(8, function() return big:goTo(12, 30) end)
planMoveAction(0, function() return level_action_showMoves("UU") end)
addm(5, "help12") -- Nesmíme ani přihazovat
planMoveAction(5, function() return small:goTo(8, 17) end)
syncTalk()
planMoveAction(5, function() return level_action_showMoves("rr") end) -- velká zemře
planMoveAction(5, function() return small:goTo(18, 26) end)
addm(0, "help13") -- nepohnu s ocelí
syncTalk()
planMoveAction(0, function() return level_action_showMoves("ll") end)
planPushingAnim(20)
planMoveAction(0, function() return level_action_showMoves("rdllll") end) -- malá umře
syncMoves()
planTimeAction(20, function() level_action_restart() end)

planTimeAction(5, function() level_action_load() end)
addv(10, "help14") -- teď ti ukážeme, co dělat umíme
syncTalk()
planMoveAction(10, function() return small:goTo(8, 25) end)
addm(0, "help15") -- můžeme zvedat a pouštět
syncTalk()
planMoveAction(5, function() return level_action_showMoves("uuur") end)
syncMoves()
addm(5, "help16") -- pokud se těsně předtím zachytí
planMoveAction(5, function() return level_action_showMoves("DDLLLLLL") end)
planMoveAction(8, function() return big:goTo(12, 30) end)
planMoveAction(5, function() return level_action_showMoves("U") end)
syncTalk()
planMoveAction(5, function() return small:goTo(10, 27) end)
planMoveAction(5, function() return level_action_showMoves("r") end)
syncMoves()
addm(0, "help17") -- a dokonce ho na sebe pouštět
planMoveAction(0, function() return level_action_showMoves("lllUU") end)
planMoveAction(5, function() return small:goTo(15, 29) end)
syncTalk()
planMoveAction(5, function() return level_action_showMoves("LLLL") end)
syncMoves()
addm(0, "help18") -- podklesnu pod ním a nebo jej nasunu na támhletu polici
planMoveAction(0, function() return small:goTo(6, 25) end)
syncTalk()
planMoveAction(0, function() return level_action_showMoves("u") end)
planMoveAction(8, function() return level_action_showMoves("d") end)
planMoveAction(15, function() return level_action_showMoves("uuuuuul") end)
syncMoves()
addm(10, "help19") -- také můžeme posouvat předmět po jiném předmětu
planMoveAction(0, function() return level_action_showMoves("rrruurr") end)
planMoveAction(8, function() return big:goTo(12, 30) end)
planMoveAction(15, function() return level_action_showMoves("UUU") end)
planMoveAction(0, function() return small:goTo(12, 22) end)
planMoveAction(5, function() return level_action_showMoves("rr") end)
planMoveAction(0, function() return small:goTo(17, 22) end)
planMoveAction(5, function() return level_action_showMoves("ll") end)
syncMoves()
addv(5, "help20") -- pod předmětem volně proplouvat a předávat si jej
syncTalk()
planMoveAction(0, function() return level_action_showMoves("RRRLL") end)
planMoveAction(0, function() return small:goTo(17, 26) end)
planMoveAction(8, function() return level_action_showMoves("lllLLLRRRl") end)
syncMoves()
addm(0, "help21") -- můžeme ho jedna z druhé shodit
planMoveAction(0, function() return small:goTo(20, 25) end)
planMoveAction(0, function() return level_action_showMoves("l") end)
syncTalk()
planMoveAction(5, function() return level_action_showMoves("RRRRRRlr") end)
syncMoves()
addv(5, "help22") -- to by bylo z pravidel asi všechno
addm(0, "help23") -- já bych to ještě shrnula
planTimeAction(0, function() demo_exit() end)
