# Fish Fillets Reborn

A new mobile-first port of Fish Fillets (© 1998 ALTAR Games, GPL-released 2002), currently targetting Android with plans for Apple and desktop.

![obrazek](https://github.com/vasekp/fish-fillets/assets/11852447/f63dd474-17aa-40e7-a4f2-a50599df5b2a)

Please note that this is a "purist", tribute version, aiming to be as close to the content of the original 1998 game as possible, only using new possibilities of today's graphics.
In other words, while using modified data files from the [Fish Fillets Next Generation](https://fillets.sourceforge.net/) project, this is **not** a "Next Generation" port.
In particular, it is

* *not open* for new level designs,
* *not open* for new translations,
 
and no such pull requests will be accepted. You are free to contribute to Next Generation which welcomes such extensions.

## Why a new version?

Originally the plan was to simply port Fish Fillets NG source code to a new platform. However, problems started to arise, like a hard-coded limitation to 10 FPS, very outdated codebase, or limitations of SDL support library, including e.g. an unspecified cap on simultaneously playing sounds. Some design decisions were made differently in NG than in the original game, like level titles, options settings, music looping, with no obvious ways to change. Also, the idea of reverting to the original levelset was there from the beginning.

In this version, Fish Fillets Reborn, hardcoded ties between display and game logic were removed. In result, the game will run in your native screen FPS and pixel resolution (limited, of course, by upscaling the original bitmaps). Motion, falling, text are much smoother and not bound to fixed keyframes. Despite thorough efforts in testing, it's possible that this fact causes some misbehaviour. Please report such problems.

Please also note that while the general form of save and solve files from FFNG was retained, the content is not compatible. In other words, a file saved in Next Generation and loaded in Reborn will not work.

## How about multiple saves or retracing steps?

Saves are possible but you only have one save slot per level. When you make progress and save again, you can not restore to the previous save point. Same if you restart the level and save earlier, or hit save instead of load (a mistake you only make once, trust me). It has been confirmed by the long history of the game, and tested in the new version, that this goal is cumbersome but achievable.

Since the target platform is mobile and mistakes happen on touch screens, a very limited undo mechanics has been implemented. It is allowed to undo a single user-initiated move action. The possibility to revert is limited to a two-second window. It is not possible to undo restart, save, load, or go-to commands this way.

## Contributing

Currently the most valuable contribution is user feedback. This comprises
* ease of using (e.g., trouble moving fish around, click areas too small to hit right, game not reacting at all),
* seemingly wrong game behaviour (e.g., an object falls through the ground),
* interference with the operating system (e.g., you can't see your notifications, switch to another app without unintended motion, or something hides parts of your screen),
* failures on specific platforms (e.g., game won't start at all, crashes when exiting, sound or video does not work)
and similar. As stated above, new levels, sound data or translations are not accepted. Changes to current levels will only be considered if they are obviously wrong.

Example: if you feel that you're bothered by the fish saying the same introduction each time you restart a level, this is not a bug. If an object suddenly disappears, or responds to something that did not happen, it probably is.

Please use Github for **all** error reporting. Ensure that you are running the most recent update and include the details of your device. Attach a screenshot if relevant. Check if the issue has not been reported before.

### Support

Fish Fillets Reborn was made for the love of the original game and desire to make it available to the world once more, nearing its 25th anniversary. It was not made for money: development was not backed by any company and you won't see any ads or popups either. Should anyone feel this nevertheless deserves some appreciation, a sponsor link (PayPal) can be found in the right column. Please only sponsor if your intentions are as pure as mine: return in in-game rewards or promotion of any kind is not offered.
