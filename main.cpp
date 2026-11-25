#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

// Constants

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

// Ball
const int BALL_SIZE = 12;

// ADDED: Keys that are pressed once (menu and game states will use these)
const int KEY_NONE = 0;
const int KEY_UP = 1;
const int KEY_DOWN = 2;
const int KEY_ENTER = 3;
const int KEY_ESC = 4;

// Creating the Variables

int bricks[ROWS][COLS];
int lives = 3;
int score = 0;

// ADDED: The key pressed in this frame, set inside the event loop
int pressedKey = KEY_NONE;

// VELOCITIES AND POSITION

float paddleX = 250.f;
float paddleY = 650.f;
float const paddleS = 8.f;

float ballX = paddleX + 44.f;
float ballY = 638.f;
float dx = 0.f;
float dy = 0.f;
bool start = false;

// Ball and Paddle Collision

void bounceOnPaddle()
{
    // check: 4 comparisons + dy > 0
    int leftB = ballX;
    int rightB = ballX + BALL_SIZE;
    int bottomB = ballY + BALL_SIZE;
    int topB = ballY;

    int leftP = paddleX;
    int rightP = paddleX + PADDLE_W;
    int bottomP = paddleY + PADDLE_H;
    int topP = paddleY;

    if (dy > 0 && leftB < rightP && rightB > leftP && bottomB > topP && topB < bottomP)
    {

        // Getting the part
        dy = -5;
        ballY = paddleY - BALL_SIZE;
        int part = (ballX + 6 - paddleX) * 4 / PADDLE_W;
        std::cout << "part: " << part << "\n";
        if (part <= 0)
        {
            dx = -6;
        }
        else if (part == 1)
        {
            dx = -3;
        }
        else if (part == 2)
        {
            dx = 3;
        }
        else
        {
            dx = 6;
        }
    }
}

// Writing getting the values in 2D Array

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

    // Inputting 2D array

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            fin >> arr[r][c];
        }
    }
    return true;
}

// Setting up Level and Font

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

// Keyboard Inputs for paddle
// Changed the values of KeyBoardInputFP
void KeyboardInputFP()
{

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        paddleX = paddleX - paddleS;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        paddleX = paddleX + paddleS;
    }

    if (paddleX < 0)
    {
        paddleX = 0;
    }

    if (paddleX > WINDOW_W - PADDLE_W)
    {
        paddleX = WINDOW_W - PADDLE_W;
    }
    if (!start)
    {
        ballX = paddleX + (PADDLE_W - BALL_SIZE) / 2;
        ballY = 638.f;
    }
}

// Starting the Velocity of the ball

void moveBall()
{
    if (!start)
    {
        return;
    }

    if (start)
    {
        ballX = ballX + dx;
    }

    if (ballX < 0 || ballX > WINDOW_W - BALL_SIZE)
    {
        ballX = ballX - dx;
        dx = -dx;
    }

    if (start)
    {
        ballY = ballY + dy;
    }

    if (ballY < HUD_H)
    {
        ballY = ballY - dy;
        dy = -dy;
    }
}

// Input Function

void handleInput(sf::RenderWindow &window)
{
    // ADDED: No key pressed unless the event loop finds one
    pressedKey = KEY_NONE;

    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto *key = event->getIf<sf::Event::KeyPressed>())
        {
            // ADDED: Saving the once-per-press keys in our own variable
            if (key->code == sf::Keyboard::Key::Up)
            {
                pressedKey = KEY_UP;
            }
            else if (key->code == sf::Keyboard::Key::Down)
            {
                pressedKey = KEY_DOWN;
            }
            else if (key->code == sf::Keyboard::Key::Enter)
            {
                pressedKey = KEY_ENTER;
            }
            else if (key->code == sf::Keyboard::Key::Escape)
            {
                pressedKey = KEY_ESC;
            }

            if (key->code == sf::Keyboard::Key::Space && !start)
            {
                start = true;
                dy = -5;
                float value = rand() % 2;
                if (value == 0)
                {
                    dx = 3.f;
                }
                else
                {
                    dx = -3.f;
                }
            }
        }
    }
}

// Update Function

void updateGame()
{
    KeyboardInputFP();
    moveBall();
    bounceOnPaddle();
}

// ADDED: Drawing a text in the middle of the window

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

// ADDED: Playfield side lines and the line under the HUD

void drawBackground(sf::RenderWindow &window)
{
    sf::RectangleShape line({2.f, (float)(WINDOW_H - HUD_H)});
    line.setFillColor(sf::Color(50, 50, 100));

    line.setPosition({0.f, (float)HUD_H});
    window.draw(line);
    line.setPosition({WINDOW_W - 2.f, (float)HUD_H});
    window.draw(line);
}

// Score Text and Lives

void drawHUD(sf::RenderWindow &window, const sf::Font &font)
{
    // HUD background strip
    sf::RectangleShape strip({(float)WINDOW_W, (float)HUD_H});
    strip.setFillColor(sf::Color(25, 25, 55));
    window.draw(strip);

    // ADDED: Line between the HUD and the playfield
    sf::RectangleShape border({(float)WINDOW_W, 2.f});
    border.setFillColor(sf::Color(110, 110, 200));
    border.setPosition({0.f, HUD_H - 2.f});
    window.draw(border);

    sf::Text scoreText(font, "Score: " + std::to_string(score), 24);
    scoreText.setPosition({10.f, 10.f});
    scoreText.setFillColor(sf::Color::White);
    window.draw(scoreText);

    // ADDED: Small label before the life circles
    sf::Text livesText(font, "Lives", 18);
    livesText.setPosition({WINDOW_W - 160.f, 15.f});
    livesText.setFillColor(sf::Color(170, 170, 210));
    window.draw(livesText);

    // One small circle for every life left
    sf::CircleShape lifeDot(8.f);
    lifeDot.setFillColor(sf::Color(255, 90, 110));
    for (int i = 0; i < lives; i++)
    {
        lifeDot.setPosition({WINDOW_W - 30.f - i * 25.f, 17.f});
        window.draw(lifeDot);
    }
}

// Drawing the bricks

void drawBricks(sf::RenderWindow &window)
{
    // Colour for each number in the bricks array (0 = no brick)
    // ADDED: softer colours than the plain ones
    sf::Color colors[4] = {sf::Color(25, 25, 45),
                           sf::Color(60, 220, 150),
                           sf::Color(255, 210, 60),
                           sf::Color(255, 75, 105)};

    // One shape only, smaller than the box so a gap shows
    sf::RectangleShape brick({BRICK_W - GAP, BRICK_H - GAP});

    // ADDED: Thin light outline on every brick
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

// Drawing the paddle

void drawPaddle(sf::RenderWindow &window)
{
    sf::RectangleShape paddle({PADDLE_W, PADDLE_H});
    paddle.setFillColor(sf::Color(60, 220, 255));
    // ADDED: Outline for the paddle
    paddle.setOutlineThickness(1.f);
    paddle.setOutlineColor(sf::Color::White);
    paddle.setPosition({paddleX, paddleY});
    window.draw(paddle);
}

// Drawing the Ball

void drawBall(sf::RenderWindow &window)
{
    sf::CircleShape ball(BALL_SIZE / 2.f);
    ball.setFillColor(sf::Color::White);
    ball.setPosition({ballX, ballY});

    window.draw(ball);
}

// ADDED: Drawing the main menu (only draws, the choice logic is ours)
// selected: 0 = New Game, 1 = Continue, 2 = Exit
// showNoSave: true shows the "No saved game" message

void drawMenu(sf::RenderWindow &window, const sf::Font &font,
              int selected, bool showNoSave)
{
    drawCenteredText(window, font, "BLOCK BUSTER", 56, 150.f, sf::Color(60, 220, 255));

    std::string options[3] = {"New Game", "Continue", "Exit"};

    for (int i = 0; i < 3; i++)
    {
        // The selected option is yellow with an arrow, the others are white
        if (i == selected)
        {
            drawCenteredText(window, font, "> " + options[i] + " <", 32,
                             280.f + i * 60.f, sf::Color(255, 210, 60));
        }
        else
        {
            drawCenteredText(window, font, options[i], 32,
                             280.f + i * 60.f, sf::Color::White);
        }
    }

    if (showNoSave)
    {
        drawCenteredText(window, font, "No saved game", 24, 500.f, sf::Color(255, 75, 105));
    }

    drawCenteredText(window, font, "Up / Down to move, Enter to choose", 18, 640.f,
                     sf::Color(170, 170, 210));
}

// ADDED: Drawing the game over / you win screen (only draws)
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

// Draw Function

void drawGame(sf::RenderWindow &window, const sf::Font &font)
{
    window.clear(sf::Color(15, 15, 35));
    drawBackground(window);
    drawBricks(window);
    drawHUD(window, font);
    drawPaddle(window);
    drawBall(window);
    window.display();
}

// Main Body
int main()
{
    // For seeding the time to random function;
    srand(time(0));
    // Window Creation
    sf::RenderWindow window(sf::VideoMode({WINDOW_W, WINDOW_H}), "Block Buster");
    window.setFramerateLimit(60);

    // Setting up Level and Font
    sf::Font font;

    if (!loadResources(font))
    {
        return -1;
    }

    // Loop for the game

    while (window.isOpen())
    {
        handleInput(window);
        updateGame();
        drawGame(window, font);
    }
}
