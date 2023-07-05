
dir_no = 0
dir_up = 1
dir_down = 2
dir_left = 3
dir_right = 4

-- -----------------------------------------------------------------
function getDirShift(dir)
    local shiftX, shiftY = 0, 0
    if dir == dir_left then
        shiftX = -1
    elseif dir == dir_right then
        shiftX = 1
    elseif dir == dir_up then
        shiftY = -1
    elseif dir == dir_down then
        shiftY = 1
    end
    return shiftX, shiftY
end
