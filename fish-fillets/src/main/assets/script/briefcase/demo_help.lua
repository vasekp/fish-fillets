function planMoveAction(delay, action) -- TODO precondition, postcondition
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

demo_enter()
addm(0, "help1") -- Teď na nic nesahej
planMoveAction(3, function() return level_action_showMoves("llll") end)
planMoveAction(0, function() return small:goTo(17, 25) end)
planMoveAction(3, function() return level_action_showMoves("LLL") end)
planMoveAction(0, function() return big:goTo(17, 24) end)
addv(0, "help2") -- Než vstoupíme do dílny
planTimeAction(0, function() game_flashButton("save") end)
planTimeAction(5, function() level_action_save() end)

planMoveAction(6, function() return small:goTo(7, 17) end)
planMoveAction(0, function() return level_action_showMoves("r") end)
planMoveAction(3, function() return level_action_showMoves("LLLL") end)
addm(0, "help3") -- co nám ubližuje
addv(0, "help4") -- Já budu dělat figuranta.
addm(0, "help5") -- neměli házet předměty
planTimeAction(3, function() return level_action_showMoves("r") end)
planMoveAction(3, function() return level_action_showMoves("r") end) -- velká zemře
planMoveAction(13, function() return small:goTo(7, 25) end)
planMoveAction(3, function() return level_action_showMoves("uuuu") end)
addm(0, "help6") -- nesmíme podklesnout
planTimeAction(0, function() return level_action_showMoves("d") end) -- malá zemře
planTimeAction(30, function() level_action_restart() end)

addv(10, "help7") -- Nyní začínáme znovu
planTimeAction(0, function() game_flashButton("load") end)
planTimeAction(5, function() level_action_load() end)

addv(8, "help8") -- jsme na místě, kde jsme hru ukládali
addm(0, "help9") -- další věc... posunovat si předmět po hřbetě
planMoveAction(0, function() return big:goTo(12, 30) end)
planTimeAction(8, function() return level_action_showMoves("UUll") end) -- velká zemře
addm(30, "help10") -- Některé předměty mají takový tvar
planMoveAction(0, function() return small:goTo(6, 25) end)
planTimeAction(5, function() return level_action_showMoves("uuuuuuul") end) -- malá zemře
planTimeAction(30, function() level_action_restart() end)

addv(10, "help11") -- Znovu nahrajeme
planTimeAction(0, function() game_flashButton("load") end)
planTimeAction(5, function() level_action_load() end)

planMoveAction(8, function() return big:goTo(12, 30) end)
planTimeAction(0, function() return level_action_showMoves("UU") end)
addm(6, "help12") -- Nesmíme ani přihazovat
planMoveAction(5, function() return small:goTo(8, 17) end)
planTimeAction(5, function() return level_action_showMoves("rr") end) -- velká zemře
planMoveAction(5, function() return small:goTo(17, 26) end)
addm(0, "help13") -- nepohnu s ocelí
planMoveAction(10, function() return level_action_showMoves("l") end) -- TODO malá tlačí
planTimeAction(20, function() return level_action_showMoves("rdllll") end) -- malá umře
planTimeAction(60, function() level_action_restart() end) -- TODO ready() + 30

planTimeAction(5, function() level_action_load() end)
addv(10, "help14") -- teď ti ukážeme, co dělat umíme
planTimeAction(10, function() return small:goTo(8, 25) end)
addm(0, "help15") -- můžeme zvedat a pouštět
planTimeAction(5, function() return level_action_showMoves("uuur") end)
addm(20, "help16") -- pokud se těsně předtím zachytí // TODO
planMoveAction(5, function() return level_action_showMoves("DDLLLLLL") end)
planMoveAction(8, function() return big:goTo(12, 30) end)
planMoveAction(5, function() return level_action_showMoves("U") end)
planTimeAction(5, function() return small:goTo(10, 27) end)
planMoveAction(5, function() return level_action_showMoves("r") end)
addm(30, "help17") -- a dokonce ho na sebe pouštět // TODO ready()
planMoveAction(0, function() return level_action_showMoves("lllUU") end)
planMoveAction(5, function() return small:goTo(15, 29) end)
planTimeAction(5, function() return level_action_showMoves("LLLL") end)
addm(40, "help18") -- podklesnu pod ním a nebo jej nasunu na támhletu polici // TODO
planMoveAction(0, function() return small:goTo(6, 25) end)
planTimeAction(8, function() return level_action_showMoves("u") end)
planMoveAction(8, function() return level_action_showMoves("d") end)
planMoveAction(15, function() return level_action_showMoves("uuuuuul") end)
addm(40, "help19") -- také můžeme posouvat předmět po jiném předmětu // TODO
planMoveAction(0, function() return level_action_showMoves("rrruurr") end)
planMoveAction(8, function() return big:goTo(12, 30) end)
planMoveAction(15, function() return level_action_showMoves("UUU") end)
planMoveAction(0, function() return small:goTo(12, 22) end)
planMoveAction(5, function() return level_action_showMoves("rr") end)
planMoveAction(0, function() return small:goTo(17, 22) end)
planMoveAction(5, function() return level_action_showMoves("ll") end)
addv(70, "help20") -- pod předmětem volně proplouvat a předávat si jej // TODO
planTimeAction(8, function() return level_action_showMoves("RRRLL") end)
planMoveAction(0, function() return small:goTo(17, 26) end)
planTimeAction(8, function() return level_action_showMoves("lllLLLRRRl") end)
addm(40, "help21") -- můžeme ho jedna z druhé shodit // TODO
planMoveAction(0, function() return small:goTo(20, 25) end)
planMoveAction(0, function() return level_action_showMoves("l") end)
planTimeAction(8, function() return level_action_showMoves("RRRRRRlr") end)
addv(30, "help22") -- to by bylo z pravidel asi všechno // TODO
addm(0, "help23") -- já bych to ještě shrnula
planTimeAction(0, function() demo_exit() end)
