
-- -----------------------------------------------------------------
-- Init
-- -----------------------------------------------------------------
local function prog_init()
    initModels()
    sound_playMusic("music/rybky03.ogg")
    local pokus = getRestartCount()


    -- -------------------------------------------------------------
    local function prog_init_room()
        local pom1, pom2, pomb1, pomb2 = 0, 0, false, false

        if pokus <= 1 then
            roompole[1] = 0
        else
            roompole[1] = roompole[1] + 1
        end
        room.uz = 30 + random(20)

        return function()
            if game_getCycles() == room.uz and isReady(small) and isReady(big) then
                if roompole[1] > 4 then
                    roompole[1] = -1
                    addm(1, "zav-m-hrac")
                    addv(9, "zav-v-zachranit")
                    room.uz = room.uz + 222 + random(1111)
                else
                    if roompole[1] > 0 then
                        roompole[1] = roompole[1] - 1
                    end
                    if roompole[1] == -1 then
                        pom1 = random(3)
                    else
                        pom1 = random(math.max(pokus + 1, 5))
                    end
                    switch(pom1){
                        [0] = function()
                            addv(1, "zav-v-sto")
                            if random(3) > 0 then
                                addv(6, "zav-v-trpyt")
                            end
                            if big.Y == big.YStart then
                                addm(9, "zav-m-pohnout")
                            end
                        end,
                        [1] = function()
                            addm(1, "zav-m-krasa")
                            addv(9, "zav-v-venku")
                        end,
                        [2] = function()
                            addv(1, "zav-v-zaval")
                            addm(9, "zav-m-hopskok")
                        end,
                        [3] = function()
                            addm(1, "zav-m-kameny")
                            addv(9, "zav-v-zeleny")
                        end,
                        [4] = function()
                            addv(1, "zav-v-restart")
                            addm(8, "zav-m-pravda")
                        end,
                    }
                    room.uz = room.uz + 666 + random(2222)
                end
            end
        end
    end

    -- -------------------------------------------------------------
    local function prog_init_drahokamy()
        local pom1, pom2, pomb1, pomb2 = 0, 0, false, false

        local gems = {}
        local colors = {}
        for pom1 = 3, 111 do
            local gem = getModelsTable()[pom1]
            gems[pom1 - 3] = gem
            colors[pom1 - 3] = random(6)
            gem.glob = -random(100)
            gem.afaze = colors[pom1 - 3] * 4
            gem:updateAnim()
        end

        return function()
            for key, gem in pairs(gems) do
                gem.glob = gem.glob + 1
                local afaze = colors[key] * 4
                if isIn(gem.glob, {1, 2, 3}) then
                    afaze = afaze + gem.glob
                elseif isIn(gem.glob, {4, 5, 6}) then
                    afaze = afaze + 7 - gem.glob
                elseif gem.glob == 7 then
                    gem.glob = -random(100) - 10
                end
                gem.afaze = afaze
                gem:updateAnim()
            end
        end
    end

    -- --------------------
    local update_table = {}
    local subinit
    subinit = prog_init_room()
    if subinit then
        table.insert(update_table, subinit)
    end
    subinit = prog_init_drahokamy()
    if subinit then
        table.insert(update_table, subinit)
    end
    return update_table
end
local update_table = prog_init()


-- -----------------------------------------------------------------
-- Update
-- -----------------------------------------------------------------
function prog_update()
    for key, subupdate in pairs(update_table) do
        subupdate()
    end
end

