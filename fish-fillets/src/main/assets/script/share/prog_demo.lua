
function planDelay(delay)
    game_planAction(function(count)
        return count >= delay
    end)
end
