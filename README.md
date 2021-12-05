# 2D-Alien-Shooting-Game
Basic 2D game where you shoot aliens with limited ammo. Detailed description of the features and problems are in the .cpp file. Game controls and rules are explained in the main menu of the game.
<br>
There is a start screen, you can choose your difficulty by pressing one of "e" "h" and "n" and if another key is pressed the 
the game gives an error sound. If hard mode is chosen health increases by 5 and ammo drops by 5. target speed changes according to the gamemode. The 
health, the gamemode, the amount of ammo left, and the amount of aliens left is displayed during gameplay, health goes down every time an ailen 
reaches the left side of the screen and if health becomes 0 the game ends. If you run out of ammo the game ends, you get a different game over screens 
according to why you died, when you win, you get a win screen. There are no boundary checks so when the player reaches the very bottom or top of the 
screen, it teleports to the oppsite side, so it gets easier to move from very top to the bottom or vice versa
