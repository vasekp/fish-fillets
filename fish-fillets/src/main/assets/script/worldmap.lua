-- Fish House

map_branch("start", "", "script/start/init.lua", 320, 121, 0xFFFFFF, {
  cs="Jak to všechno začalo",
  en="How It All Started"})

map_node("briefcase", "start", "script/briefcase/init.lua", 329, 153, {
  cs="Podivná zásilka",
  en="Briefcase Message"})

map_node("cellar", "briefcase", "script/cellar/init.lua", 320, 189, {
  cs="Zkouška ve sklepě",
  en="Rehearsal in Cellar"})

map_node("library", "cellar", "script/library/init.lua", 301, 224, {
  cs="Co zbylo z knihovny",
  en="Library Flotsam"})

map_node("stairs", "library", "script/stairs/init.lua", 285, 252, {
  cs="Zeleň na schodišti",
  en="Plants on the Stairs"})

map_node("broom", "stairs", "script/broom/init.lua", 279, 286, {
  cs="Nepořádek v kotelně",
  en="A Mess in the Boiler Room"})

map_node("reef", "broom", "script/reef/init.lua", 292, 315, {
  cs="Pod útesem",
  en="Under the Reef"})

map_node("wc", "reef", "script/wc/init.lua", 314, 338, {
  cs="Zamčeni na WC",
  en="Closed in the Closet"})

-- Ship Wrecks

map_branch("submarine", "library", "script/submarine/init.lua", 340, 228, 0xFFFF00, {
  cs="Utonulá ponorka",
  en="Drowned Submarine"})

map_node("party1", "submarine", "script/party1/init.lua", 381, 224, {
  cs="Výletní loď",
  en="Picnic Boat"})

map_node("elk", "party1", "script/elk/init.lua", 422, 210, {
  cs="Velká vlastenecká válka",
  en="Great War"})

map_node("wreck", "elk", "script/wreck/init.lua", 456, 189, {
  cs="Loď kapitána Silvera",
  en="The Ship of Captain Silver"})

map_node("viking1", "wreck", "script/viking1/init.lua", 483, 158, {
  cs="Poslední plavba",
  en="The Last Voyage"})

map_node("airplane", "viking1", "script/airplane/init.lua", 491, 119, {
  cs="Výška: -9000 stop",
  en="Altitude: Minus 9000 Feet"})

map_node("bathyscaph", "airplane", "script/bathyscaph/init.lua", 477, 84, {
  cs="Batyskaf",
  en="Bathyscaph"})

map_node("tank", "bathyscaph", "script/tank/init.lua", 446, 58, {
  cs="Obojživelný tank",
  en="Amphibious Tank"})

map_node("viking2", "tank", "script/viking2/init.lua", 402, 61, {
  cs="Osm vikingů a pes",
  en="Eight Vikings in a Boat"})

map_node("party2", "viking2", "script/party2/init.lua", 372, 88, {
  cs="Návrat z výletu",
  en="Return from the Party"})

map_node("gods", "party2", "script/gods/init.lua", 391, 124, {
  cs="Bohové musí být šílení",
  en="The Gods Must Be Mad"},
  "script/gods/demo_poster.lua")

-- Silver's Ship

map_branch("cabin1", "wreck", "script/cabin1/init.lua", 464, 227, 0x00FF00, {
  cs="Kajuta prvního důstojníka",
  en="The First Mate’s Cabin"})

map_node("snowman", "cabin1", "script/snowman/init.lua", 487, 256, {
  cs="Zimní jídelna",
  en="The Winter Mess Hall"})

map_node("cannons", "snowman", "script/cannons/init.lua", 521, 271, {
  cs="Pal!",
  en="Fire!"})

map_node("kitchen", "cannons", "script/kitchen/init.lua", 558, 254, {
  cs="Lodní kuchyně",
  en="Ship Kitchen"})

map_node("cabin2", "kitchen", "script/cabin2/init.lua", 571, 210, {
  cs="Kajuta druhého důstojníka",
  en="Second Mate’s Cabin"})

map_node("captain", "cabin2", "script/captain/init.lua", 550, 175, {
  cs="Kapitánova kajuta",
  en="Captain’s Cabin"})

map_node("map", "captain", "script/map/init.lua", 510, 199, {
  cs="Silverova tajná skrýš",
  en="Silver’s Hideout"},
  "script/map/demo_poster.lua")

-- City In the Deep

map_branch("elevator1", "stairs", "script/elevator1/init.lua", 247, 234, 0x000000, {
  cs="Dům s výtahem",
  en="House with an Elevator"})

map_node("city", "elevator1", "script/city/init.lua", 219, 217, {
  cs="Vítejte ve městě!",
  en="Welcome To Our City"})

map_node("ufo", "city", "script/ufo/init.lua", 192, 192, {
  cs="Den nezávislosti",
  en="Independence Day"})

map_node("columns", "ufo", "script/columns/init.lua", 171, 161, {
  cs="Na kolonádě",
  en="The Columns"})

map_node("pavement", "columns", "script/pavement/init.lua", 161, 125, {
  cs="Nerovná dlažba",
  en="Uneven Pavement"})

map_node("pyramid", "pavement", "script/pyramid/init.lua", 175, 81, {
  cs="Dům pana Cheopse",
  en="Mr. Cheops’ House"})

map_node("music", "pyramid", "script/music/init.lua", 207, 52, {
  cs="Pro potěšení",
  en="A Bit of Music"})

map_node("crabshow", "music", "script/crabshow/init.lua", 244, 44, {
  cs="Panoptikum korýšů",
  en="Crab Freak Show"})

map_node("elevator2", "crabshow", "script/elevator2/init.lua", 266, 77, {
  cs="Druhý dům s výtahem",
  en="Another elevator"})

map_node("atlantis", "elevator2", "script/atlantis/init.lua", 251, 115, {
  cs="Tak takhle to bylo",
  en="And How It Was"},
  "script/atlantis/demo_poster.lua")

-- UFO

map_branch("reactor", "ufo", "script/reactor/init.lua", 167, 222, 0xFF0000, {
  cs="Reaktor",
  en="Power Plant"})

map_node("magnet", "reactor", "script/magnet/init.lua", 132, 238, {
  cs="Neznámé síly",
  en="Strange Forces"})

map_node("engine", "magnet", "script/engine/init.lua", 95, 236, {
  cs="Brm brm...",
  en="Brm... Brm..."})

map_node("steel", "engine", "script/steel/init.lua", 70, 210, {
  cs="Ocel; samá ocel",
  en="Nothing but steel"})

map_node("corridor", "steel", "script/corridor/init.lua", 78, 169, {
  cs="Přísně střežená chodba",
  en="Guarded Corridor"})

map_node("experiments", "corridor", "script/experiments/init.lua", 111, 159, {
  cs="Biologické experimenty",
  en="Biological Experiments"})

map_node("propulsion", "experiments", "script/propulsion/init.lua", 118, 192, {
  cs="Skutečný pohon",
  en="The Real Propulsion"},
  "script/propulsion/demo_poster.lua")

-- Coral reef

map_branch("corals", "reef", "script/corals/init.lua", 329, 294, 0x00FFFF, {
  cs="První bizarnosti",
  en="First Bizarre Things"})

map_node("labyrinth", "corals", "script/labyrinth/init.lua", 367, 296, {
  cs="Bludiště",
  en="Labyrinth"})

map_node("imprisoned", "labyrinth", "script/imprisoned/init.lua", 401, 314, {
  cs="Uvězněni",
  en="Imprisoned"})

map_node("society", "imprisoned", "script/society/init.lua", 418, 346, {
  cs="Uzavřená společnost",
  en="Closed Society"})

map_node("creatures", "society", "script/creatures/init.lua", 412, 383, {
  cs="Spící tvorečkové",
  en="Sleeping Creatures"})

map_node("cancan", "creatures", "script/cancan/init.lua", 386, 406, {
  cs="Kankánoví krabíci",
  en="Cancan Crabs"})

map_node("pearls", "cancan", "script/pearls/init.lua", 352, 408, {
  cs="Ještě jednu perličku!",
  en="One More Pearl; Please!"})

map_node("turtle", "pearls", "script/turtle/init.lua", 337, 375, {
  cs="Telepatická mrcha",
  en="Telepathic Devil"},
  "script/turtle/demo_poster.lua")

-- Treasure Cave

map_branch("aztec", "labyrinth", "script/aztec/init.lua", 407, 282, 0x0000FF, {
  cs="Sál aztéckého umění",
  en="Aztec Art Hall"})

map_node("gems", "aztec", "script/gems/init.lua", 441, 291, {
  cs="Blýskavý zával",
  en="Shiny Cave-in"})

map_node("chest", "gems", "script/chest/init.lua", 469, 319, {
  cs="Truhlička obra Koloděje",
  en="Giant’s Chest"})

map_node("alibaba", "chest", "script/alibaba/init.lua", 494, 341, {
  cs="Alibabův sál",
  en="The Hall of Ali-baba"})

map_node("cave", "alibaba", "script/cave/init.lua", 529, 334, {
  cs="Nejhlubší jeskyně",
  en="The Deepest Cave"})

map_node("grail", "cave", "script/grail/init.lua", 558, 318, {
  cs="Artuš by se divil",
  en="What Would King Arthur Say?"},
  "script/grail/demo_poster.lua")

-- Dump

map_branch("computer", "wc", "script/computer/init.lua", 289, 368, 0xFF00FF, {
  cs="Hlubinný server",
  en="The Deep Server"})

map_node("noground", "computer", "script/noground/init.lua", 257, 397, {
  cs="Skoro žádná zeď",
  en="Almost No Wall"})

map_node("bathroom", "noground", "script/bathroom/init.lua", 217, 415, {
  cs="Instalatérský odpad",
  en="Plumbman’s Refuse"})

map_node("duckie", "bathroom", "script/duckie/init.lua", 170, 415, {
  cs="Dobrodružství s kachničkou",
  en="Adventure with Pink Duckie"})

map_node("puzzle", "duckie", "script/puzzle/init.lua", 138, 385, {
  cs="Rozstříhaná čmáranice",
  en="Shredded Stickman"})

map_node("dump", "puzzle", "script/dump/init.lua", 154, 342, {
  cs="Opravdový chaos",
  en="Real Chaos"})

map_node("barrel", "dump", "script/barrel/init.lua", 192, 351, {
  cs="Greenpeace by zuřili",
  en="Outraged Greenpeace"},
  "script/barrel/demo_poster.lua")

-- Secret Computer

map_branch("tetris", "computer", "script/tetris/init.lua", 254, 349, 0x800000, {
  cs="TETRIS",
  en="TETRIS"})

map_node("emulator", "tetris", "script/emulator/init.lua", 226, 317, {
  cs="Emulátor",
  en="Emulator"})

map_node("warcraft", "emulator", "script/warcraft/init.lua", 198, 290, {
  cs="Garden of War",
  en="Garden of War"})

map_node("windoze", "warcraft", "script/windoze/init.lua", 162, 279, {
  cs="Oblíbené položky",
  en="Favorites"})

map_node("hardware", "windoze", "script/hardware/init.lua", 129, 290, {
  cs="Hardwarový problém",
  en="A Hardware Problem"})

map_node("floppy", "hardware", "script/floppy/init.lua", 97, 314, {
  cs="Read only",
  en="Read Only"},
  "script/floppy/demo_poster.lua")

-- Ending

map_ending("ending", "script/ending/init.lua", 320, 50, 0x808080, {
  cs="Doma",
  en="At Home"},
  "script/ending/demo_poster.lua")
