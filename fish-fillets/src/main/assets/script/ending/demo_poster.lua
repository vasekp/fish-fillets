
codename = "ending"
file_include("script/"..codename.."/demo_dialogs.lua")

-- -----------------------------------------------------------------
file_include("script/share/prog_demo.lua")
slide_display("images/"..codename.."/poster.png", 0, 0)

planTalk("dlg-x-poster1")
planSpace()
planTalk("dlg-x-poster2")
planTalk("dlg-x-boss")
planSpace()
planTalk("dlg-x-poster3")
planTalk("dlg-x-poster4")
planStop()

