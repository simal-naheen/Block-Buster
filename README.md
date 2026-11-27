# Block Buster

A brick breaker game in C++ with SFML 3.0.2.
Made for CS1002 Programming Fundamentals, Fall 2026, FAST NUCES Lahore.

![Screenshot](screenshot.png)

## Team

| Name | Roll Number |
|---|---|
| Muhammad Aazmeer | 26L-0051 |
| Simal Naheen | 26L-0028 |

Both members worked on the whole project together. The table below shows who
leads the explanation of each feature in the demo.

| Feature | Demo responsibility |
|---|---|
| 1. Game window and game loop | Simal Naheen |
| 2. Bricks from a text file | Simal Naheen |
| 3. Paddle and launching the ball | Simal Naheen |
| 6. Lives, score and HUD | Simal Naheen |
| 7. Menu and game states | Simal Naheen |
| 4. Ball movement and bouncing | Muhammad Aazmeer |
| 5. Breaking bricks (corner collision) | Muhammad Aazmeer |
| 8. Save and continue | Muhammad Aazmeer |
| 10. Code quality | Muhammad Aazmeer |
| Bonus: Power-ups | Muhammad Aazmeer |

## How to Play

Break all the bricks with the ball. Keep the ball in play with the paddle.
If the ball falls below the paddle you lose a life. You start with 3 lives.

- Hitting a brick gives 10 points.
- The hit that breaks a brick gives 50 points.
- Bricks have 1, 2 or 3 hits (green, yellow, red).

## Controls

| Key | Action |
|---|---|
| Left / A | Move paddle left |
| Right / D | Move paddle right |
| Space | Launch the ball |
| Esc | Save the game and go back to the menu |
| Up / Down | Move in the menu |
| Enter | Choose in the menu, go back to the menu from Game Over / You Win |

## Power-ups (Bonus)

When a brick breaks there is a 1 in 5 chance that a power-up falls.
Only one can fall at a time.

- **W**: paddle becomes 150 pixels wide for 10 seconds.
- **+**: one extra life (maximum 5 lives).

## How to Run

1. Install Visual Studio with the "Desktop development with C++" workload.
2. Set up SFML 3.0.2 as shown in the official tutorial:
   https://www.sfml-dev.org/tutorials/3.0/getting-started/visual-studio/
3. Clone this repository.
4. Open the project in Visual Studio and build it (x64, Debug or Release).
5. Make sure `level.txt` and `arial.ttf` are in the working directory
   (the folder of the `.vcxproj`, or next to the `.exe`).
6. Run the game.

## Project Files

| File | Purpose |
|---|---|
| `main.cpp` | The whole game |
| `level.txt` | Brick layout (first line `8 10`, then 8 rows of numbers 0 to 3) |
| `arial.ttf` | Font used for the text |
| `savegame.txt` | Created when you press Esc while playing |

## Level File Format

Each number is how many hits the brick needs:
`0` = no brick, `1` = green, `2` = yellow, `3` = red.

## Notes

- The window is 600 x 700 pixels at 60 FPS.
- A finished game (Game Over or You Win) empties `savegame.txt`.
- If `level.txt` is missing the game shows an error and does not crash.
