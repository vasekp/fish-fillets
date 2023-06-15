
local function charAt(s, i)
    return string.sub(s, i, i)
end
    
local function moveXY(model, symbols, destX, destY)
    -- symbol is e.g. 'UDLR'
    local result = false
    local locX, locY = model:getLoc()
    local w = model:getW()
    local h = model:getH()

    --print("TEST: move '"..symbols.."' "..locX..","..locY.." -> "..destX..","..destY)

    --TODO: find undirect path too
    if locX > destX and isWater(locX - 1, locY) then
        level_action_move(charAt(symbols, 3))
    elseif locX < destX and isWater(locX + w, locY) then
        level_action_move(charAt(symbols, 4))
    elseif locY > destY and isWater(locX, locY - 1) then
        level_action_move(charAt(symbols, 1))
    elseif locY < destY and isWater(locX, locY + h) then
        level_action_move(charAt(symbols, 2))
    else
        if locX ~= destX or locY ~= destY then
            error("moveXY - fish cannot move")
        end
        result = true
    end
    return result
end
local function bigMoveXY(x, y)
    return moveXY(big, 'UDLR', x, y)
end
local function smallMoveXY(x, y)
    return moveXY(small, 'udlr', x, y)
end
local function planDelay(delay)
    level_planShow(function(count)
        return count >= delay
    end)
end
local function waitDialogs()
    level_planShow(function(count)
        return no_dialog()
    end)
end

local function showMove(move)
    level_planShow(function(count)
        return level_action_move(move)
    end)
end

--TODO: F1 for How to Play manual
-- ---------------------------------------------------------------
level_planShow(function(count)
    addm(0, "help1") -- Teď na nic nesahej
    return true
end)
planDelay(3)
level_planShow(function(count)
    return smallMoveXY(17, 25)
end)
showMove('L')
showMove('L')
showMove('L')

level_planShow(function(count)
    addv(0, "help2") -- Než vstoupíme do dílny
    return true
end)
level_planShow(function(count)
    return bigMoveXY(18, 21)
end)
showMove('D')
showMove('L')
showMove('D')
waitDialogs() -- help1, help2
level_planShow(function(count)
    return level_action_save()
end)
planDelay(6)
level_planShow(function(count)
    return smallMoveXY(9, 25)
end)

level_planShow(function(count)
    addm(0, "help3") -- co nám ubližuje
    return true
end)
showMove('u')
showMove('u')
showMove('l')
showMove('l')
level_planShow(function(count)
    return smallMoveXY(7, 17)
end)
planDelay(3)
showMove('r')

level_planShow(function(count)
    addv(0, "help4") -- Já budu dělat figuranta.
    return true
end)
showMove('L')
showMove('L')
showMove('L')
showMove('L')
waitDialogs() -- help3, help4

level_planShow(function(count)
    addm(0, "help5") -- neměli házet předměty
    return true
end)
waitDialogs() -- help5
showMove('r')
planDelay(3)
showMove('r') -- big dies here
planDelay(13)
showMove('l')
showMove('l')

level_planShow(function(count)
    addm(0, "help6") -- nesmíme podklesnout
    return true
end)
showMove('d')
showMove('d')
showMove('r')
level_planShow(function(count)
    return smallMoveXY(9, 25)
end)
planDelay(1)
level_planShow(function(count)
    return smallMoveXY(7, 25)
end)
planDelay(6)
showMove('u')
showMove('u')
showMove('u')
showMove('u')
waitDialogs() -- help6
planDelay(19)
showMove('d') -- small dies
planDelay(39)

level_planShow(function(count)
    return level_action_restart()
end)
planDelay(11)

level_planShow(function(count)
    addv(0, "help7") -- Nyní začínáme znovu
    return true
end)
waitDialogs() -- help7
level_planShow(function(count)
    return level_action_load()
end)
planDelay(8)
level_planShow(function(count)
    addv(0, "help8") -- jsme na místě, kde jsme hru ukládali
    return true
end)
waitDialogs() -- help8
level_planShow(function(count)
    addm(0, "help9") -- posunovat si předmět po hřbetě
    return true
end)
level_planShow(function(count)
    return bigMoveXY(9, 23)
end)
showMove('D')
showMove('D')
showMove('L')
showMove('L')
level_planShow(function(count)
    return bigMoveXY(7, 30)
end)
showMove('R')
showMove('R')
showMove('R')
planDelay(3)
showMove('U')
planDelay(3)
showMove('U')
planDelay(10)
waitDialogs() -- help9
showMove('l')
planDelay(26)
showMove('l')
planDelay(3)
level_planShow(function(count)
    return smallMoveXY(7, 25)
end)
planDelay(3)

level_planShow(function(count)
    addm(0, "help10") -- Některé předměty mají takový tvar
    return true
end)
planDelay(5)
showMove('u')
showMove('u')
showMove('u')
showMove('u')
showMove('u')
showMove('u')
showMove('u')
waitDialogs() -- help10
showMove('l')
planDelay(5)
showMove('l') -- small dies
planDelay(44)
level_planShow(function(count)
    return level_action_restart()
end)
planDelay(12)

level_planShow(function(count)
    addv(0, "help11") -- Znovu nahrajeme
    return true
end)
waitDialogs() -- help11
level_planShow(function(count)
    return level_action_load()
end)
planDelay(8)
level_planShow(function(count)
    return bigMoveXY(9, 23)
end)
showMove('D')
showMove('D')
showMove('L')
showMove('L')
level_planShow(function(count)
    return bigMoveXY(7, 30)
end)
showMove('R')
showMove('R')
showMove('R')
planDelay(2)
showMove('U')
planDelay(1)
showMove('U')
planDelay(6)

level_planShow(function(count)
    addm(0, "help12") -- Nesmíme ani přihazovat
    return true
end)
planDelay(4)
showMove('l')
showMove('u')
showMove('l')
showMove('u')
planDelay(1)
level_planShow(function(count)
    return smallMoveXY(8, 23)
end)
planDelay(1)
level_planShow(function(count)
    return smallMoveXY(8, 17)
end)
planDelay(7)
showMove('r')
planDelay(8)
waitDialogs() -- help12
showMove('r')
planDelay(3)
showMove('r') -- big dies
showMove('r')
showMove('r')
showMove('r')
showMove('d')
showMove('d')
showMove('r')
showMove('d')
showMove('d')
showMove('d')
planDelay(1)
showMove('r')
showMove('r')
level_planShow(function(count)
    return smallMoveXY(17, 25)
end)

level_planShow(function(count)
    addm(0, "help13") -- nepohnu s ocelí
    return true
end)
planDelay(7)
showMove('d')
showMove('l')
waitDialogs() -- help13
showMove('l')
showMove('l')
planDelay(40)
showMove('r')
showMove('d')
planDelay(2)
showMove('l')
showMove('l')
showMove('l')
showMove('l') -- small dies
planDelay(45)

level_planShow(function(count)
    return level_action_restart()
end)
planDelay(8)
level_planShow(function(count)
    return level_action_load()
end)
planDelay(4)

level_planShow(function(count)
    addv(0, "help14") -- teď ti ukážeme, co dělat umíme
    return true
end)
planDelay(10)
waitDialogs() -- help14
level_planShow(function(count)
    return smallMoveXY(7, 25)
end)
level_planShow(function(count)
    addm(0, "help15") -- můžeme zvedat a pouštět
    return true
end)
showMove('u')
showMove('u')
showMove('u')
planDelay(7)
waitDialogs() -- help15
showMove('r')
showMove('r')
showMove('r')
planDelay(5)
planDelay(2)
showMove('L')
showMove('D')
showMove('D')
planDelay(2)
showMove('L')
showMove('L')
showMove('L')
showMove('L')
showMove('L')
planDelay(1)
showMove('L')
showMove('L')
showMove('L')
showMove('L')
showMove('D')
showMove('D')
showMove('D')
showMove('D')
showMove('D')
showMove('R')
showMove('R')
showMove('R')
showMove('U')
planDelay(2)

level_planShow(function(count)
    addm(0, "help16") -- pokud se těsně předtím zachytí
    return true
end)
planDelay(10)
showMove('d')
showMove('d')
showMove('d')
showMove('l')
showMove('l')
showMove('l')
showMove('d')
showMove('d')
showMove('r')
planDelay(7)
waitDialogs() -- help16
showMove('r')
planDelay(3)
level_planShow(function(count)
    return smallMoveXY(4, 27)
end)
showMove('d')
showMove('d')
showMove('d')
planDelay(4)
planDelay(3)
showMove('U')
planDelay(2)
showMove('U')
planDelay(10)
level_planShow(function(count)
    return smallMoveXY(10, 30)
end)
showMove('u')
showMove('r')
level_planShow(function(count)
    addm(0, "help17") -- a dokonce ho na sebe pouštět
    return true
end)
waitDialogs() -- help17
showMove('L')
showMove('L')
showMove('L')
planDelay(8)
showMove('D')
level_planShow(function(count)
    return bigMoveXY(3, 28)
end)
planDelay(11)

level_planShow(function(count)
    addm(0, "help18") -- podklesnu pod ním a nebo jej nasunu na támhletu polici
    return true
end)
planDelay(4)
-- 2205
level_planShow(function(count)
    return smallMoveXY(7, 29)
end)
level_planShow(function(count)
    return smallMoveXY(7, 25)
end)
showMove('l')
planDelay(3)
showMove('u')
planDelay(6)
waitDialogs() -- help18
showMove('d')
planDelay(16)
showMove('u')
showMove('u')
showMove('u')
showMove('u')
showMove('u')
planDelay(4)
showMove('u')
planDelay(4)
showMove('l')
planDelay(5)

showMove('r')
showMove('r')
showMove('r')
showMove('u')
showMove('u')
planDelay(5)
showMove('r')
showMove('r')
planDelay(7)
level_planShow(function(count)
    return bigMoveXY(8, 28)
end)
showMove('D')
showMove('R')
planDelay(5)
showMove('U')
planDelay(1)
showMove('U')
showMove('U')
planDelay(14)

level_planShow(function(count)
    addm(0, "help19") -- také můžeme posouvat předmět po jiném předmětu
    return true
end)
planDelay(4)
showMove('l')
showMove('l')
showMove('d')
showMove('d')
planDelay(1)
showMove('r')
showMove('r')
showMove('d')
showMove('d')
showMove('d')
planDelay(8)
showMove('r')
planDelay(2)
showMove('r')
level_planShow(function(count)
    return smallMoveXY(6, 22)
end)
level_planShow(function(count)
    return smallMoveXY(6, 28)
end)
planDelay(1)
level_planShow(function(count)
    return smallMoveXY(14, 28)
end)
showMove('u')
showMove('r')
showMove('r')
showMove('r')
level_planShow(function(count)
    return smallMoveXY(17, 22)
end)
planDelay(2)
showMove('l')
planDelay(1)
showMove('l')
planDelay(9)

waitDialogs() -- help19
level_planShow(function(count)
    addv(0, "help20") -- pod předmětem volně proplouvat a předávat si jej
    return true
end)
showMove('r')
showMove('r')
planDelay(6)
showMove('R')
showMove('R')
showMove('R')
planDelay(3)
showMove('L')
showMove('L')
planDelay(5)
level_planShow(function(count)
    return smallMoveXY(17, 26)
end)
showMove('l')
showMove('l')
showMove('l')
planDelay(6)
showMove('L')
showMove('L')
showMove('L')
planDelay(4)
level_planShow(function(count)
    return bigMoveXY(10, 26)
end)
planDelay(7)
level_planShow(function(count)
    return smallMoveXY(18, 26)
end)
planDelay(15)
showMove('u')
planDelay(2)
showMove('l')
planDelay(1)

waitDialogs() -- help20
level_planShow(function(count)
    addm(0, "help21") -- můžeme ho jedna z druhé shodit
    return true
end)
planDelay(9)
level_planShow(function(count)
    return bigMoveXY(15, 26)
end)
planDelay(4)
showMove('R')
planDelay(12)
planDelay(5)
waitDialogs() -- help21
showMove('l')
planDelay(8)
showMove('r')
planDelay(3)

level_planShow(function(count)
    addv(0, "help22") -- to by bylo z pravidel asi všechno
    addm(0, "help23") -- já bych to ještě shrnula
    return true
end)
