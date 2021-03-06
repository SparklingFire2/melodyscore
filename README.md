Warning :
Never open melodyscore.exe while the game is running, that is likely to cause issues.
You should go to C:\Users\Your username\AppData\Roaming\MelodyEscape
and make a copy of hs_4.mel (file storing custom difficulty scores) to be able to restore it if something goes wrong.
Don't call your copy hs_t.mel because that's a temporary file used by my program so it would be deleted.
hs_4.mel is the only game file modified by my program so there's no need to copy anything else.

How to use :
Open melodyscore.exe, the program will display "Done!" when it's finished.
Press enter to exit.
You should now see one file for each difficulty, open any of them.
Each line goes like this : Track name : Your best score / Best possible score : Your best score in percentage of the best possible score (Number of hearts)
with (6) meaning 5 golden hearts
The file is ordered from your best to your worst track (using the percentage)

This program allows you to have separate scores for separate custom difficulties.
The file 5 - Custom.txt only shows scores from your currently selected custom difficulty.

In order for this to work properly, you need to follow these instructions :
Don't delete the file CustomCurrentDifficulty.txt (generated by my program).
Everytime you change anything on the Custom Difficulty Selection Screen(*) and press play,
do this before playing any song :
Close the game (don't simply minimize it, that will cause issues) and run melodyscore.exe.
You can now restart the game and play the custom difficulty you selected before closing the game.

(*) For your first time selecting a custom difficulty, you need to do it too even if you pressed play
without changing anything.

If you already had custom difficulty scores before opening melodyscore.exe :
If all your scores were in the same custom difficulty, it's not an issue, simply open melodyscore.exe once.
Unfortunately if you had scores in multiple difficulties or don't remember, you should go to
C:\Users\Your username\AppData\Roaming\MelodyEscape and delete hs_4.mel.
If you don't delete it, some songs (possibly a few, possibly a lot) won't be
correctly associated with a custom difficulty.