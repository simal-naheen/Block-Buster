#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

// ============================================================
// Constants
// ============================================================

// Window
const int WINDOW_W = 600;
const int WINDOW_H = 700;
const int HUD_H = 50;

// Brick
const int ROWS = 8;
const int COLS = 10;
const int BRICK_W = 60;
const int BRICK_H = 25;
const int BRICKS_Y = 100;
const float GAP = 2.f;

// Paddle
const int PADDLE_W = 100;
const int PADDLE_H = 14;
const float PADDLE_Y = 650.f;
const float PADDLE_START_X = 250.f;
const float PADDLE_SPEED = 8.f;
const int PADDLE_PARTS = 4; // the paddle is split in 4 parts for the bounce

// Ball
const int BALL_SIZE = 12;
const float BALL_SPEED_Y = 5.f;
const float BALL_DX_SLOW = 3.f;
const float BALL_DX_FAST = 6.f;

// Rules
const int START_LIVES = 3;
const int SCORE_HIT = 10;
const int SCORE_BREAK = 50;

// Power-ups (bonus)
const int POWERUP_SIZE = 24;
const float POWERUP_SPEED = 3.f;
const int POWERUP_CHANCE = 5; // 1 in 5 broken bricks drops a power-up
const int MAX_POWERUPS = 10;
const char POWERUP_WIDE = 'W';
const char POWERUP_LIFE = '+';
const int WIDE_PADDLE_W = 150;
const int WIDE_PADDLE_FRAMES = 600;

// Keys that are pressed once (menu and game states use these)
const int KEY_NONE = 0;
const int KEY_UP = 1;
const int KEY_DOWN = 2;
const int KEY_ENTER = 3;
const int KEY_ESC = 4;
const int KEY_SPACE = 5;

// Game states
const int STATE_MENU = 0;
const int STATE_PLAYING = 1;
const int STATE_GAME_OVER = 2;

// Menu
const int MENU_NEW_GAME = 0;
const int MENU_CONTINUE = 1;
const int MENU_EXIT = 2;
const int MENU_COUNT = 3;
const float MENU_FIRST_Y = 280.f;
const float MENU_SPACING = 60.f;

// Text positions
const float LAUNCH_HINT_Y = 675.f;

// ============================================================
// Variables
// ============================================================

int bricks[ROWS][COLS];
int lives = START_LIVES;
int score = 0;

// The key pressed in this frame, set inside the event loop
int pressedKey = KEY_NONE;

// Paddle and ball
float paddleX = PADDLE_START_X;
int paddleW = PADDLE_W;
int wideFramesLeft = 0;

float ballX = PADDLE_START_X + (PADDLE_W - BALL_SIZE) / 2;
float ballY = PADDLE_Y - BALL_SIZE;
float dx = 0.f;
float dy = 0.f;
bool start = false;

// Falling power-ups (one slot per power-up)
bool powerActive[MAX_POWERUPS];
float powerX[MAX_POWERUPS];
float powerY[MAX_POWERUPS];
char powerType[MAX_POWERUPS];

// Game state
int gameState = STATE_MENU;
int menuSelected = MENU_NEW_GAME;
bool showNoSave = false;
std::string endMessage = "";

// ============================================================
// Loading the level and resources
// ============================================================

bool loadLevel(int arr[][COLS])
{
    std::ifstream fin("level.txt");

    // Checking the file
    if (!fin)
    {
        std::cout << "File Not Found\n";
        return false;
    }

    // Checking the rows and cols
    int fileRows, fileCols;
    fin >> fileRows >> fileCols;

    if (fileRows != ROWS || fileCols != COLS)
    {
        std::cout << "Wrong Level Size\n";
        return false;
    }

    // Reading the 2D array
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            fin >> arr[r][c];
        }
    }
    return true;
}

bool loadResources(sf::Font &font)
{
    if (!loadLevel(bricks))
    {
        return false;
    }

    if (!font.openFromFile("arial.ttf"))
    {
        std::cout << "Font not Loaded!\n";
        return false;
    }
    return true;
}

// ============================================================
// Input
// ============================================================

// Turns an SFML key into one of our own key constants
int toGameKey(sf::Keyboard::Key code)
{
    if (code == sf::Keyboard::Key::Up)
    {
        return KEY_UP;
    }
    if (code == sf::Keyboard::Key::Down)
    {
        return KEY_DOWN;
    }
    if (code == sf::Keyboard::Key::Enter)
    {
        return KEY_ENTER;
    }
    if (code == sf::Keyboard::Key::Escape)
    {
        return KEY_ESC;
    }
    if (code == sf::Keyboard::Key::Space)
    {
        return KEY_SPACE;
    }
    return KEY_NONE;
}

void handleInput(sf::RenderWindow &window)
{
    // No key pressed unless the event loop finds one
    pressedKey = KEY_NONE;

    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto *key = event->getIf<sf::Event::KeyPressed>())
        {
            pressedKey = toGameKey(key->code);
        }
    }
}

// ============================================================
// Ball and bricks collision
// ============================================================

// Is there a brick under this point? Gives back its row and col
bool cornerHitsBrick(float cx, float cy, int &r, int &c)
{
    // Only the brick area counts
    if (cx < 0 || cx >= COLS * BRICK_W ||
        cy < BRICKS_Y || cy >= BRICKS_Y + ROWS * BRICK_H)
    {
        return false;
    }

    r = (cy - BRICKS_Y) / BRICK_H;
    c = cx / BRICK_W;

    return bricks[r][c] > 0;
}

// Checks the 4 corners of the ball, stops at the first brick found
bool findBrickHit(int &r, int &c)
{
    // The last pixel of the ball is at + BALL_SIZE - 1
    int leftB = ballX;
    int rightB = ballX + BALL_SIZE - 1;
    int bottomB = ballY + BALL_SIZE - 1;
    int topB = ballY;

    if (cornerHitsBrick(leftB, topB, r, c))
    {
        return true;
    }
    if (cornerHitsBrick(rightB, topB, r, c))
    {
        return true;
    }
    if (cornerHitsBrick(leftB, bottomB, r, c))
    {
        return true;
    }
    if (cornerHitsBrick(rightB, bottomB, r, c))
    {
        return true;
    }
    return false;
}

// ============================================================
// Power-ups (bonus)
// ============================================================

// Removes all falling power-ups and the wide paddle effect
void resetPowerUps()
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        powerActive[i] = false;
    }
    paddleW = PADDLE_W;
    wideFramesLeft = 0;
}

// Drops a random power-up from the middle of the broken brick
void spawnPowerUp(int r, int c)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!powerActive[i])
        {
            powerActive[i] = true;
            powerX[i] = c * BRICK_W + (BRICK_W - POWERUP_SIZE) / 2;
            powerY[i] = BRICKS_Y + r * BRICK_H;

            if (rand() % 2 == 0)
            {
                powerType[i] = POWERUP_WIDE;
            }
            else
            {
                powerType[i] = POWERUP_LIFE;
            }
            return;
        }
    }
}

// The effect of a caught power-up
void applyPowerUp(char type)
{
    if (type == POWERUP_WIDE)
    {
        paddleW = WIDE_PADDLE_W;
        wideFramesLeft = WIDE_PADDLE_FRAMES;
    }
    else
    {
        lives++;
    }
}

// Does power-up number i overlap the paddle?
bool powerUpTouchesPaddle(int i)
{
    return powerX[i] < paddleX + paddleW &&
           powerX[i] + POWERUP_SIZE > paddleX &&
           powerY[i] + POWERUP_SIZE > PADDLE_Y &&
           powerY[i] < PADDLE_Y + PADDLE_H;
}

// Moves every power-up down, catches it or lets it fall away
void updatePowerUps()
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!powerActive[i])
        {
            continue;
        }

        powerY[i] = powerY[i] + POWERUP_SPEED;

        if (powerUpTouchesPaddle(i))
        {
            applyPowerUp(powerType[i]);
            powerActive[i] = false;
        }
        else if (powerY[i] > WINDOW_H)
        {
            powerActive[i] = false;
        }
    }
}

// Counts down the wide paddle and shrinks it back at 0
void updateWidePaddle()
{
    if (wideFramesLeft > 0)
    {
        wideFramesLeft--;

        if (wideFramesLeft == 0)
        {
            paddleW = PADDLE_W;
        }
    }
}

// ============================================================
// Hitting bricks
// ============================================================

// One hit on a brick: take one hit off, add the score, maybe drop a power-up
void hitBrick(int r, int c)
{
    bricks[r][c]--;

    if (bricks[r][c] == 0)
    {
        score += SCORE_BREAK;

        if (rand() % POWERUP_CHANCE == 0)
        {
            spawnPowerUp(r, c);
        }
    }
    else
    {
        score += SCORE_HIT;
    }
}

// ============================================================
// Paddle and ball movement
// ============================================================

void movePaddle()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        paddleX = paddleX - PADDLE_SPEED;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        paddleX = paddleX + PADDLE_SPEED;
    }

    // Keeping the paddle inside the window
    if (paddleX < 0)
    {
        paddleX = 0;
    }

    if (paddleX > WINDOW_W - paddleW)
    {
        paddleX = WINDOW_W - paddleW;
    }

    // Before launch the ball sits in the middle of the paddle
    if (!start)
    {
        ballX = paddleX + (paddleW - BALL_SIZE) / 2;
        ballY = PADDLE_Y - BALL_SIZE;
    }
}

// Is the ball overlapping the paddle?
bool ballTouchesPaddle()
{
    int leftB = ballX;
    int rightB = ballX + BALL_SIZE;
    int bottomB = ballY + BALL_SIZE;
    int topB = ballY;

    int leftP = paddleX;
    int rightP = paddleX + paddleW;
    int bottomP = PADDLE_Y + PADDLE_H;
    int topP = PADDLE_Y;

    return leftB < rightP && rightB > leftP && bottomB > topP && topB < bottomP;
}

// The part of the paddle that was hit decides dx
void bounceOnPaddle()
{
    // Only a falling ball bounces
    if (dy <= 0 || !ballTouchesPaddle())
    {
        return;
    }

    dy = -BALL_SPEED_Y;
    ballY = PADDLE_Y - BALL_SIZE;

    // Which part of the paddle was hit (0 to 3)
    int part = (ballX + BALL_SIZE / 2 - paddleX) * PADDLE_PARTS / paddleW;

    if (part <= 0)
    {
        dx = -BALL_DX_FAST;
    }
    else if (part == 1)
    {
        dx = -BALL_DX_SLOW;
    }
    else if (part == 2)
    {
        dx = BALL_DX_SLOW;
    }
    else
    {
        dx = BALL_DX_FAST;
    }
}

// Puts the ball back on the paddle, waiting for Space
void resetBall()
{
    start = false;
}

void launchBall()
{
    start = true;
    dy = -BALL_SPEED_Y;

    if (rand() % 2 == 0)
    {
        dx = BALL_DX_SLOW;
    }
    else
    {
        dx = -BALL_DX_SLOW;
    }
}

// x step: undo it if the ball touched a wall or a brick
void moveBallX()
{
    int r = 0;
    int c = 0;

    ballX = ballX + dx;

    if (ballX < 0 || ballX > WINDOW_W - BALL_SIZE)
    {
        ballX = ballX - dx;
        dx = -dx;
    }
    else if (findBrickHit(r, c))
    {
        hitBrick(r, c);
        ballX = ballX - dx;
        dx = -dx;
    }
}

// y step: undo it if the ball touched the top or a brick
void moveBallY()
{
    int r = 0;
    int c = 0;

    ballY = ballY + dy;

    if (ballY < HUD_H)
    {
        ballY = ballY - dy;
        dy = -dy;
    }
    else if (findBrickHit(r, c))
    {
        hitBrick(r, c);
        ballY = ballY - dy;
        dy = -dy;
    }
}

void moveBall()
{
    if (!start)
    {
        return;
    }

    moveBallX();
    moveBallY();
}

// ============================================================
// Winning and losing
// ============================================================

bool allBricksGone()
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (bricks[r][c] > 0)
            {
                return false;
            }
        }
    }
    return true;
}

// ============================================================
// Save and continue
// ============================================================

// Line 1: score lives, then the 8 rows of bricks
void saveGame()
{
    std::ofstream fout("savegame.txt");

    fout << score << " " << lives << "\n";

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            fout << bricks[r][c] << " ";
        }
        fout << "\n";
    }
}

// Gives back false if there is no usable save
bool loadGame()
{
    std::ifstream fin("savegame.txt");

    int savedScore, savedLives;
    int savedBricks[ROWS][COLS];

    // Missing or empty file
    if (!(fin >> savedScore >> savedLives))
    {
        return false;
    }

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (!(fin >> savedBricks[r][c]))
            {
                return false;
            }
        }
    }

    // A save with no lives is not a game in progress
    if (savedLives <= 0)
    {
        return false;
    }

    // The file is fine, now the real variables change
    score = savedScore;
    lives = savedLives;

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            bricks[r][c] = savedBricks[r][c];
        }
    }

    paddleX = PADDLE_START_X;
    start = false;
    dx = 0.f;
    dy = 0.f;
    resetPowerUps();
    gameState = STATE_PLAYING;
    return true;
}

// Opening the file for writing empties it
void clearSave()
{
    std::ofstream fout("savegame.txt");
}

// ============================================================
// Game states
// ============================================================

void startNewGame()
{
    if (!loadLevel(bricks))
    {
        return;
    }

    lives = START_LIVES;
    score = 0;
    paddleX = PADDLE_START_X;
    start = false;
    dx = 0.f;
    dy = 0.f;
    resetPowerUps();
    gameState = STATE_PLAYING;
}

void endGame(const std::string &message)
{
    clearSave();
    endMessage = message;
    gameState = STATE_GAME_OVER;
}

// Menu keys: Up, Down, Enter
void menuInput(sf::RenderWindow &window)
{
    if (pressedKey == KEY_UP)
    {
        menuSelected--;
        if (menuSelected < 0)
        {
            menuSelected = MENU_COUNT - 1;
        }
        showNoSave = false;
    }
    else if (pressedKey == KEY_DOWN)
    {
        menuSelected++;
        if (menuSelected >= MENU_COUNT)
        {
            menuSelected = 0;
        }
        showNoSave = false;
    }
    else if (pressedKey == KEY_ENTER)
    {
        if (menuSelected == MENU_NEW_GAME)
        {
            startNewGame();
        }
        else if (menuSelected == MENU_CONTINUE)
        {
            if (!loadGame())
            {
                showNoSave = true;
            }
        }
        else
        {
            window.close();
        }
    }
}

// What the pressed key means in the current state
void handleStateInput(sf::RenderWindow &window)
{
    if (gameState == STATE_MENU)
    {
        menuInput(window);
    }
    else if (gameState == STATE_PLAYING)
    {
        if (pressedKey == KEY_ESC)
        {
            saveGame();
            gameState = STATE_MENU;
        }
        else if (pressedKey == KEY_SPACE && !start)
        {
            launchBall();
        }
    }
    else if (gameState == STATE_GAME_OVER)
    {
        if (pressedKey == KEY_ENTER)
        {
            menuSelected = MENU_NEW_GAME;
            gameState = STATE_MENU;
        }
    }
}

// ============================================================
// Update
// ============================================================

void updatePlaying()
{
    movePaddle();
    moveBall();
    bounceOnPaddle();
    updatePowerUps();
    updateWidePaddle();

    // Ball fell below the window
    if (ballY > WINDOW_H)
    {
        lives--;
        resetBall();
    }

    if (lives == 0)
    {
        endGame("GAME OVER");
    }
    else if (allBricksGone())
    {
        endGame("YOU WIN");
    }
}

void updateGame()
{
    // Menu and end screen have nothing to update
    if (gameState == STATE_PLAYING)
    {
        updatePlaying();
    }
}

// ============================================================
// Drawing
// ============================================================

// Drawing a text in the middle of the window
void drawCenteredText(sf::RenderWindow &window, const sf::Font &font,
                      const std::string &str, int size, float y,
                      sf::Color color = sf::Color::White)
{
    sf::Text text(font, str, size);
    text.setFillColor(color);

    // Moving the origin to the middle of the text so it sits in the centre
    sf::FloatRect box = text.getLocalBounds();
    text.setOrigin({box.position.x + box.size.x / 2.f, 0.f});
    text.setPosition({WINDOW_W / 2.f, y});
    window.draw(text);
}

// Playfield side lines
void drawBackground(sf::RenderWindow &window)
{
    sf::RectangleShape line({2.f, (float)(WINDOW_H - HUD_H)});
    line.setFillColor(sf::Color(50, 50, 100));

    line.setPosition({0.f, (float)HUD_H});
    window.draw(line);
    line.setPosition({WINDOW_W - 2.f, (float)HUD_H});
    window.draw(line);
}

// HUD strip with the line under it
void drawHudBackground(sf::RenderWindow &window)
{
    sf::RectangleShape strip({(float)WINDOW_W, (float)HUD_H});
    strip.setFillColor(sf::Color(25, 25, 55));
    window.draw(strip);

    sf::RectangleShape border({(float)WINDOW_W, 2.f});
    border.setFillColor(sf::Color(110, 110, 200));
    border.setPosition({0.f, HUD_H - 2.f});
    window.draw(border);
}

// One small circle for every life left
void drawLives(sf::RenderWindow &window, const sf::Font &font)
{
    sf::Text livesText(font, "Lives", 18);
    livesText.setPosition({WINDOW_W - 160.f, 15.f});
    livesText.setFillColor(sf::Color(170, 170, 210));
    window.draw(livesText);

    sf::CircleShape lifeDot(8.f);
    lifeDot.setFillColor(sf::Color(255, 90, 110));
    for (int i = 0; i < lives; i++)
    {
        lifeDot.setPosition({WINDOW_W - 30.f - i * 25.f, 17.f});
        window.draw(lifeDot);
    }
}

// Score text and lives
void drawHUD(sf::RenderWindow &window, const sf::Font &font)
{
    drawHudBackground(window);

    sf::Text scoreText(font, "Score: " + std::to_string(score), 24);
    scoreText.setPosition({10.f, 10.f});
    scoreText.setFillColor(sf::Color::White);
    window.draw(scoreText);

    drawLives(window, font);
}

void drawBricks(sf::RenderWindow &window)
{
    // Colour for each number in the bricks array (0 = no brick)
    sf::Color colors[4] = {sf::Color(25, 25, 45),
                           sf::Color(60, 220, 150),
                           sf::Color(255, 210, 60),
                           sf::Color(255, 75, 105)};

    // One shape only, smaller than the box so a gap shows
    sf::RectangleShape brick({BRICK_W - GAP, BRICK_H - GAP});

    // Thin light outline on every brick
    brick.setOutlineThickness(1.f);
    brick.setOutlineColor(sf::Color(255, 255, 255, 120));

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            // Skipping the empty cells
            if (bricks[r][c] == 0)
            {
                continue;
            }

            brick.setFillColor(colors[bricks[r][c]]);

            // Box position never changes, GAP / 2 puts the shape in the middle of the box
            brick.setPosition({c * BRICK_W + GAP / 2,
                               BRICKS_Y + r * BRICK_H + GAP / 2});

            window.draw(brick);
        }
    }
}

void drawPaddle(sf::RenderWindow &window)
{
    sf::RectangleShape paddle({(float)paddleW, (float)PADDLE_H});
    paddle.setFillColor(sf::Color(60, 220, 255));
    paddle.setOutlineThickness(1.f);
    paddle.setOutlineColor(sf::Color::White);
    paddle.setPosition({paddleX, PADDLE_Y});
    window.draw(paddle);
}

void drawBall(sf::RenderWindow &window)
{
    sf::CircleShape ball(BALL_SIZE / 2.f);
    ball.setFillColor(sf::Color::White);
    ball.setPosition({ballX, ballY});
    window.draw(ball);
}

// Small hint under the paddle area while the ball is waiting
void drawLaunchHint(sf::RenderWindow &window, const sf::Font &font)
{
    drawCenteredText(window, font, "Press SPACE to launch", 18, LAUNCH_HINT_Y,
                     sf::Color(170, 170, 210));
}

// Draws one falling power-up capsule at (x, y)
// type: 'W' = wide paddle (blue), '+' = extra life (green)
void drawPowerUp(sf::RenderWindow &window, const sf::Font &font,
                 float x, float y, char type)
{
    sf::RectangleShape box({(float)POWERUP_SIZE, (float)POWERUP_SIZE});
    if (type == POWERUP_WIDE)
    {
        box.setFillColor(sf::Color(60, 140, 255));
    }
    else
    {
        box.setFillColor(sf::Color(60, 220, 150));
    }
    box.setOutlineThickness(1.f);
    box.setOutlineColor(sf::Color::White);
    box.setPosition({x, y});
    window.draw(box);

    sf::Text letter(font, std::string(1, type), 18);
    letter.setFillColor(sf::Color::White);
    sf::FloatRect b = letter.getLocalBounds();
    letter.setOrigin({b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f});
    letter.setPosition({x + POWERUP_SIZE / 2.f, y + POWERUP_SIZE / 2.f});
    window.draw(letter);
}

// Draws every falling power-up
void drawPowerUps(sf::RenderWindow &window, const sf::Font &font)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (powerActive[i])
        {
            drawPowerUp(window, font, powerX[i], powerY[i], powerType[i]);
        }
    }
}

// Main menu (only draws, the choice logic is ours)
// selected: 0 = New Game, 1 = Continue, 2 = Exit
// showNoSave: true shows the "No saved game" message
void drawMenu(sf::RenderWindow &window, const sf::Font &font,
              int selected, bool showNoSave)
{
    drawCenteredText(window, font, "BLOCK BUSTER", 56, 150.f, sf::Color(60, 220, 255));

    std::string options[MENU_COUNT] = {"New Game", "Continue", "Exit"};

    for (int i = 0; i < MENU_COUNT; i++)
    {
        float y = MENU_FIRST_Y + i * MENU_SPACING;

        // The selected option is yellow with an arrow, the others are white
        if (i == selected)
        {
            drawCenteredText(window, font, "> " + options[i] + " <", 32, y,
                             sf::Color(255, 210, 60));
        }
        else
        {
            drawCenteredText(window, font, options[i], 32, y, sf::Color::White);
        }
    }

    if (showNoSave)
    {
        drawCenteredText(window, font, "No saved game", 24, 500.f, sf::Color(255, 75, 105));
    }

    drawCenteredText(window, font, "Up / Down to move, Enter to choose", 18, 640.f,
                     sf::Color(170, 170, 210));
}

// Game over / you win screen (only draws)
// message: "GAME OVER" or "YOU WIN"
void drawEndScreen(sf::RenderWindow &window, const sf::Font &font,
                   const std::string &message, int finalScore)
{
    // Dark layer over the screen
    sf::RectangleShape shade({(float)WINDOW_W, (float)WINDOW_H});
    shade.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(shade);

    sf::Color messageColor = sf::Color(255, 75, 105);
    if (message == "YOU WIN")
    {
        messageColor = sf::Color(60, 220, 150);
    }

    drawCenteredText(window, font, message, 64, 220.f, messageColor);
    drawCenteredText(window, font, "Final Score: " + std::to_string(finalScore), 32, 320.f);
    drawCenteredText(window, font, "Press Enter", 24, 400.f, sf::Color(255, 210, 60));
}

// Bricks, HUD, paddle, ball and power-ups
void drawPlayfield(sf::RenderWindow &window, const sf::Font &font)
{
    drawBackground(window);
    drawBricks(window);
    drawHUD(window, font);
    drawPaddle(window);
    drawBall(window);
    drawPowerUps(window, font);

    if (!start && gameState == STATE_PLAYING)
    {
        drawLaunchHint(window, font);
    }
}

void drawGame(sf::RenderWindow &window, const sf::Font &font)
{
    window.clear(sf::Color(15, 15, 35));

    if (gameState == STATE_MENU)
    {
        drawMenu(window, font, menuSelected, showNoSave);
    }
    else if (gameState == STATE_PLAYING)
    {
        drawPlayfield(window, font);
    }
    else if (gameState == STATE_GAME_OVER)
    {
        drawPlayfield(window, font);
        drawEndScreen(window, font, endMessage, score);
    }

    window.display();
}

// ============================================================
// Main
// ============================================================

int main()
{
    // Seeding the random function
    srand(time(0));

    // Window creation
    sf::RenderWindow window(sf::VideoMode({WINDOW_W, WINDOW_H}), "Block Buster");
    window.setFramerateLimit(60);

    // One KeyPressed event per press, so the menu moves one step at a time
    window.setKeyRepeatEnabled(false);

    // Setting up level and font
    sf::Font font;

    if (!loadResources(font))
    {
        return -1;
    }

    // No power-ups falling at the start
    resetPowerUps();

    // Game loop
    while (window.isOpen())
    {
        handleInput(window);
        handleStateInput(window);
        updateGame();
        drawGame(window, font);
    }
}