README

WordWorx - By Tony Smolar
-------------------------
Wordworx was inspired by the Wordsmith game included on TiVos.

Gameplay
--------
The objective is to create words from the 12x7 pile of letters and 
maximize your score.

Words must be a minimum of three letters and a maximum of twelve.

The words are scored by totalling up the value of the letters and multiplying
it by the number of letters.

Letter values an frequency are exactly the same as the board game
Scrabble(TM).   However, note that not all the letters will be present in the 
pile.  (84 in WordWorx vs 100 in Scrabble(TM)).    Also there are no blanks.

The word dictionary used is based on the SOWPODS official Scrabble(TM)
tournamemt dictionary.   Some words that are impossible to make in Wordworx
have been removed for compactness.

Interface
---------
Wordworx can be played with either the mouse or keyboard.  

When you build a word that is acceptable according to the dictionary, a green
OK icon appears in the top right.

Mouse Usage-
   Click on a letter on top of the pile to add to the word
   
   Click on the word to remove a letter
   
   Click on the "OK" Icon or the "Accept" button to accept a word
   
   Click on the "Clear" Button to delete all the letters from a word
   
   Click on Quit to exit back to main menu
   
Keyboard Usage-

   You can use the TAB key or the Arrow keys to move the yellow cursor around
   to select letters
   
   ENTER - selects letter or button under yellow cursor.
   
   BACKSPACE - remove the last letter from the word
   
   SPACE - Accept word
   
   ESCAPE - Clear whole Word
   
   Q - Quit back to main menu
   
   M - Mute Sound 
   
   H - Give hint
   
Hiscores
--------
   Wordworx stores both the 10 top game scores, and the 15 top individual
   word scores.
   
   As of Version 1.0.9, scores can also be stored and read from a central
   internet server.
   
   At the end of a game, you will see a list of the words you made as well
   as their point values.   If they are a high enough value to make the 
   hiscore table, a * will appear next to them.   Note that it's possible that
   not all your *'d words will actually appear in the high score table.
   This occurs if they are pushed out by other words that you made.
   
   NOTE ON INTERNET SCORES:
   When the game starts up, it will attempt to read hiscores from the
   internet.  If it succeeds, then 4 high score tables will be displayed
   instead of two.
   
   At the end of a game, it will attempt to send scores to the internet
   server.  If it can't connect, then it will cache the hiscore information
   until later.   Everytime the game is started, it will attempt to submit
   the cached scores (if any).    If you play offline, and want to submit
   your score, just start the game while online.   You can exit as soon as
   the title screen comes up.
   
   Note that the internet hiscore function can slow the game startup time, 
   depending on how slow the connection is.   If cached scores are being
   submitted, startup may be even slower.   A progress bar is shown as this
   occurs.
   
   No personal information apart from the name you enter, your score and
   your words are submitted.
   
   The internet hiscore functionality can be disabled by adding -n to the 
   startup.
   
Command Line Options
--------------------
   Wordworx accepts the following command line options
   
   '-f' Run in full screen mode (default)
   '-w' Run in a window
   '-v' Print version information and exit
   '-m' Mute Sound
   '-n' Do Not send hiscores through internet

Release Notes
-------------
   As of version 1.1.1, the stability of the internet score reading/writing
   has been improved.   However occasional crashes are still possible.  If
   this occurs, rerun the game.   We'll try to address the problem in a future
   release.
   
   If you experience persistant problems, you may want to consider the -n 
   option to disable this feature

   Also, as of version 1.1.1,  a high-score file is no longer distributed
   with WordWorx,  this means that if you uninstall and install a new version
   to the same directory, your high scores SHOULD be preserved.

Windows Notes
-------------

Linux Notes
-----------
   Linux can be compiled to use either the SDL or Allegro graphics libraries
   Currently SDL is a little better supported.
   
Zaurus Notes
------------
   Wordworx can be played with the touchscreen or the keyboard.  It is
   compiled to use the bvdd SDL library.   Some optimizations were done
   to improve gamespeed on these devices.
