
codename = "barrel"
file_include("script/share/level_dialog.lua")
dialogLoad("script/"..codename.."/demo_")

-- -----------------------------------------------------------------
file_include("script/share/prog_demo.lua")
demo_display("images/"..codename.."/poster.jpg", 0, 0)

planTalk("dlg-x-poster1")
planTalk("dlg-x-poster2")
planTalk("dlg-x-poster3")
planSpace()
planTalk("dlg-x-poster4")
planStop()

