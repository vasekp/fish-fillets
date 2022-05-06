
-- branch_addNode(parent, codename, datafile, x, y, color=-1, poster="")
-- Fish House
branch_addNode("", "start", "script/start/init.lua", 320, 121)
branch_addNode("start", "briefcase", "script/briefcase/init.lua", 329, 153)
branch_addNode("briefcase", "cellar", "script/cellar/init.lua", 320, 189)
branch_addNode("cellar", "library", "script/library/init.lua", 301, 224, 0xFFFF00)
branch_addNode("library", "stairs", "script/stairs/init.lua", 285, 252, 0x000000)
branch_addNode("stairs", "broom", "script/broom/init.lua", 279, 286)
branch_addNode("broom", "reef", "script/reef/init.lua", 292, 315, 0x00FFFF)
branch_addNode("reef", "wc", "script/wc/init.lua", 314, 338, 0xFF00FF)

-- Ship Wrecks
branch_addNode("library", "submarine", "script/submarine/init.lua", 340, 228)
branch_addNode("submarine", "party1", "script/party1/init.lua", 381, 224)
branch_addNode("party1", "elk", "script/elk/init.lua", 422, 210)
branch_addNode("elk", "wreck", "script/wreck/init.lua", 456, 189, 0x00FF00)
branch_addNode("wreck", "viking1", "script/viking1/init.lua", 483, 158)
branch_addNode("viking1", "airplane", "script/airplane/init.lua", 491, 119)
branch_addNode("airplane", "bathyscaph", "script/bathyscaph/init.lua", 477, 84)
branch_addNode("bathyscaph", "tank", "script/tank/init.lua", 446, 58)
branch_addNode("tank", "viking2", "script/viking2/init.lua", 402, 61)
branch_addNode("viking2", "party2", "script/party2/init.lua", 372, 88)
branch_addNode("party2", "gods", "script/gods/init.lua", 391, 124,
        -1, "script/gods/demo_poster.lua")

-- Silver's Ship
branch_addNode("wreck", "cabin1", "script/cabin1/init.lua", 464, 227)
branch_addNode("cabin1", "snowman", "script/snowman/init.lua", 487, 256)
branch_addNode("snowman", "cannons", "script/cannons/init.lua", 521, 271)
branch_addNode("cannons", "kitchen", "script/kitchen/init.lua", 558, 254)
branch_addNode("kitchen", "cabin2", "script/cabin2/init.lua", 571, 210)
branch_addNode("cabin2", "captain", "script/captain/init.lua", 550, 175)
branch_addNode("captain", "map", "script/map/init.lua", 510, 199,
        -1, "script/map/demo_poster.lua")

-- City In the Deep
branch_addNode("stairs", "elevator1", "script/elevator1/init.lua", 247, 234)
branch_addNode("elevator1", "city", "script/city/init.lua", 219, 217)
branch_addNode("city", "ufo", "script/ufo/init.lua", 192, 192, 0xFF0000)
branch_addNode("ufo", "columns", "script/columns/init.lua", 171, 161)
branch_addNode("columns", "pavement", "script/pavement/init.lua", 161, 125)
branch_addNode("pavement", "pyramid", "script/pyramid/init.lua", 175, 81)
branch_addNode("pyramid", "music", "script/music/init.lua", 207, 52)
branch_addNode("music", "crabshow", "script/crabshow/init.lua", 244, 44)
branch_addNode("crabshow", "elevator2", "script/elevator2/init.lua", 266, 77)
branch_addNode("elevator2", "atlantis", "script/atlantis/init.lua", 251, 115,
        -1, "script/atlantis/demo_poster.lua")

-- UFO
branch_addNode("ufo", "reactor", "script/reactor/init.lua", 167, 222)
branch_addNode("reactor", "magnet", "script/magnet/init.lua", 132, 238)
branch_addNode("magnet", "engine", "script/engine/init.lua", 95, 236)
branch_addNode("engine", "steel", "script/steel/init.lua", 70, 210)
branch_addNode("steel", "corridor", "script/corridor/init.lua", 78, 169)
branch_addNode("corridor", "experiments", "script/experiments/init.lua", 111, 159)
branch_addNode("experiments", "propulsion", "script/propulsion/init.lua", 118, 192,
        -1, "script/propulsion/demo_poster.lua")

-- Coral reef
branch_addNode("reef", "corals", "script/corals/init.lua", 329, 294)
branch_addNode("corals", "labyrinth", "script/labyrinth/init.lua", 367, 296, 0x0000FF)
branch_addNode("labyrinth", "imprisoned", "script/imprisoned/init.lua", 401, 314)
branch_addNode("imprisoned", "society", "script/society/init.lua", 418, 346)
branch_addNode("society", "creatures", "script/creatures/init.lua", 412, 383)
branch_addNode("creatures", "cancan", "script/cancan/init.lua", 386, 406)
branch_addNode("cancan", "pearls", "script/pearls/init.lua", 352, 408)
branch_addNode("pearls", "turtle", "script/turtle/init.lua", 337, 375,
        -1, "script/turtle/demo_poster.lua")

-- Treasure Cave
branch_addNode("labyrinth", "aztec", "script/aztec/init.lua", 407, 282)
branch_addNode("aztec", "gems", "script/gems/init.lua", 441, 291)
branch_addNode("gems", "chest", "script/chest/init.lua", 469, 319)
branch_addNode("chest", "alibaba", "script/alibaba/init.lua", 494, 341)
branch_addNode("alibaba", "cave", "script/cave/init.lua", 529, 334)
branch_addNode("cave", "grail", "script/grail/init.lua", 558, 318,
        -1, "script/grail/demo_poster.lua")

-- Dump
branch_addNode("wc", "computer", "script/computer/init.lua", 289, 368, 0x800000)
branch_addNode("computer", "noground", "script/noground/init.lua", 257, 397)
branch_addNode("noground", "bathroom", "script/bathroom/init.lua", 217, 415)
branch_addNode("bathroom", "duckie", "script/duckie/init.lua", 170, 415)
branch_addNode("duckie", "puzzle", "script/puzzle/init.lua", 138, 385)
branch_addNode("puzzle", "dump", "script/dump/init.lua", 154, 342)
branch_addNode("dump", "barrel", "script/barrel/init.lua", 192, 351,
        -1, "script/barrel/demo_poster.lua")

-- Secret Computer
branch_addNode("computer", "tetris", "script/tetris/init.lua", 254, 349)
branch_addNode("tetris", "emulator", "script/emulator/init.lua", 226, 317)
branch_addNode("emulator", "warcraft", "script/warcraft/init.lua", 198, 290)
branch_addNode("warcraft", "windoze", "script/windoze/init.lua", 162, 279)
branch_addNode("windoze", "hardware", "script/hardware/init.lua", 129, 290)
branch_addNode("hardware", "floppy", "script/floppy/init.lua", 97, 314,
        -1, "script/floppy/demo_poster.lua")


-- branch_setEnding(codename, datafile, poster="")
-- Ending
branch_setEnding("ending", "script/ending/init.lua",
    "script/ending/demo_poster.lua")


-- -----------------------------------------------------------------
file_include("override/worlddesc.lua")
file_include("override/worldfame.lua")

