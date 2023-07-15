
function stdBublesLoad()
    sound_addSound("bubbles", "sound/share/sp-bubbles1.ogg")
    sound_addSound("bubbles", "sound/share/sp-bubbles2.ogg")
    sound_addSound("bubbles", "sound/share/sp-bubbles3.ogg")
    sound_addSound("bubbles", "sound/share/sp-bubbles4.ogg")
    sound_addSound("bubbles", "sound/share/sp-bubbles5.ogg")
    sound_addSound("bubbles", "sound/share/sp-bubbles6.ogg")
end

function stdBubles()
    --NOTE: original was (random(100) < 5)
    if random(100) < 2 then
        sound_playSound("bubbles", 15)
    end
end

