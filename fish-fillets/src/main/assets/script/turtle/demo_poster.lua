
codename = "turtle"
file_include("script/"..codename.."/demo_dialogs.lua")

-- -----------------------------------------------------------------
file_include("script/share/prog_demo.lua")
slide_display("images/"..codename.."/poster.png", 0, 0)

planTalk("dlg-x-poster1")
planTalk("dlg-x-poster2")
planSpace()
planTalk("dlg-x-poster3")
planStop()

