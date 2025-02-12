local function syncMoves()
    game_planAction(function() return level_isReady() end)
end

-- -----------------------------------------------------------------
-- Init
-- -----------------------------------------------------------------
local function prog_init()
    initModels()
    sound_playMusic("music/rybky04.ogg")
    local pokus = getRestartCount()


    -- -------------------------------------------------------------
    local function prog_init_room()
        local pom1, pom2, pomb1, pomb2 = 0, 0, false, false

        room.uvod = 0
        room.hlaska = 0
        zavermode = true

        return function()
            if room.uvod < 4 and pom2 == 0 then
                pom2 = 1
                demo_enter()
            elseif room.uvod == 4 then
                demo_exit()
            end
            if room.uvod == 0 then
                room.uvod = 1
                addv(20, "z-v-doma")
                addm(10, "z-m-pocit")
                planDialogSet(random(10) + 5, "bar-x-suckano", 202, pldik, "cinnost")
                addv(random(20) + 10, "z-v-sef")
                addm(5, "z-m-nemluv")
                addv(2, "z-v-slyset")
                addm(6, "z-m-netusi")
                planSet(room, "hlaska", 1)
            elseif room.uvod == 2 then
                room.uvod = 3
                adddel(5)
                addm(5, "z-m-dlouho")
                planBusy(big, true)
                addv(10, "z-v-pozdrav")
                planTimeAction(0, function() small:goTo(14, 16) end)
                syncMoves()
                planBusy(small, true)
                planBusy(big, false)
                addm(5, "z-m-oblicej")
                planTimeAction(0, function() big:goTo(15, 15) end)
                addv(0, "z-v-forky")
                adddel(2)
                syncMoves()
                planBusy(big, true)
                planBusy(small, true)
                planDialog(TALK_INDEX_BOTH, 2, "z-o-blahoprejeme")
                adddel(20)
                planBusy(big, false)
                planBusy(small, false)
                planSet(room, "uvod", 4)
            end
            if room.hlaska == 1 then
                room.hlaska = 2
                room.cas = math.floor(0.5 +
                        options_getInt("playtime") / 3600)
                room:talk("z-c-konkretne@"..room.cas)
            end
            if room.hlaska >= 1 and not room:isTalking() then
                pom1 = room.hlaska - 1
                room.hlaska = room.hlaska + 1
                if room.cas >= 1000 then
                    if room.cas >= 2000 then
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-"..(math.floor(room.cas / 1000)))
                        end
                    end
                    if isIn(math.floor(room.cas / 1000), {2, 3, 4}) then
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-tisice")
                        end
                    else
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-tisic")
                        end
                    end
                end
                switch(math.floor((room.cas % 1000) / 100)){
                    [0] = function()
                    end,
                    [1] = function()
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-100")
                        end
                    end,
                    [2] = function()
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-200")
                        end
                    end,
                    [3] = function()
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-"..(math.floor((room.cas % 1000) / 100)))
                        end
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-sta")
                        end
                    end,
                    [4] = function()
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-"..(math.floor((room.cas % 1000) / 100)))
                        end
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-sta")
                        end
                    end,
                    default = function()
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-"..(math.floor((room.cas % 1000) / 100)))
                        end
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-set")
                        end
                    end,
                }
                if room.cas % 100 < 20 and room.cas % 100 > 0 then
                    pom1 = pom1 - 1
                    if pom1 == 0 then
                        room:talk("z-c-"..(room.cas % 100))
                    end
                elseif room.cas % 100 >= 20 then
                    pom1 = pom1 - 1
                    if pom1 == 0 then
                        room:talk("z-c-"..(math.floor((room.cas % 100) / 10) * 10))
                    end
                    if room.cas % 10 > 0 then
                        pom1 = pom1 - 1
                        if pom1 == 0 then
                            room:talk("z-c-"..(room.cas % 10))
                        end
                    end
                end
                pom1 = pom1 - 1
                if pom1 == 0 then
                    room.hlaska = 0
                    room:talk("z-c-hodin")
                    room.uvod = 2
                end
            end
        end
    end

    -- -------------------------------------------------------------
    local function prog_init_pldik()
        local pom1, pom2, pomb1, pomb2 = 0, 0, false, false

        pldik.cinnost = 0
        pldik.oci = 0
        pldik.suckani = 0
        local ACTOR_SUCK = 251

        return function()
            switch(pldik.cinnost){
                [0] = function()
                    if random(1000) < 5 then
                        pldik.cinnost = 1
                    end
                    if random(100) < 5 then
                        pldik.oci = random(5)
                        if pldik.oci > 0 then
                            pldik.oci = pldik.oci + 1
                        end
                    end
                    if pldik.suckani == 0 and random(100) < 2 then
                        pldik.suckani = random(5) + 1
                        pldik.suckfaze = 0
                    end
                end,
                [1] = function()
                    if random(1000) < 10 then
                        pldik.cinnost = 0
                    end
                    pldik.oci = 6
                    if pldik.suckani == 0 and random(100) < 2 then
                        pldik.suckani = random(4) + 1
                        pldik.suckfaze = 0
                    end
                end,
                [202] = function()
                    model_killSound(ACTOR_SUCK)
                    if not pldik:isTalking() then
                        pldik.cinnost = 0
                    end
                    pldik.oci = 0
                    if pldik.suckani == 0 then
                        pldik.suckani = 1000
                        pldik.suckfaze = 0
                    end
                end,
            }
            pldik.afaze = pldik.oci * 2
            if random(100) < 5 then
                pldik.afaze = 12
            end
            if pldik.suckani > 0 then
                switch(pldik.suckfaze){
                    [0] = function()
                        if pldik.cinnost < 200 then
                            model_talk(ACTOR_SUCK, "bar-x-suck"..random(4), CLASS_SOUND)
                        end
                    end,
                    [1] = function()
                        pldik.afaze = pldik.afaze + 1
                    end,
                    [2] = function()
                        pldik.afaze = pldik.afaze + 1
                    end,
                    [3] = function()
                        pldik.afaze = pldik.afaze + 1
                    end,
                    [5] = function()
                        pldik.suckani = pldik.suckani - 1
                    end,
                }
                pldik.suckfaze = (pldik.suckfaze + 1) % 6
            else
                model_killSound(ACTOR_SUCK)
            end
            pldik:updateAnim()
        end
    end

    -- --------------------
    local update_table = {}
    local subinit
    subinit = prog_init_room()
    if subinit then
        table.insert(update_table, subinit)
    end
    subinit = prog_init_pldik()
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

