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

// Creating the Variables

int bricks[ROWS][COLS];
int lives = 3;
int score = 0;

// VELOCITIES AND POSITION

float paddleX = 250.f;
float paddleY = 650.f;
float const paddleS = 8.f;

float ballX = paddleX + 44.f;
float ballY = 638.f;
float dx = 0.f;
float dy = 0.f;
bool start = false;

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

void KeyboardInputFP()
{

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        paddleX = paddleX - paddleS;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
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
        dy = -dy;
    }
}

// Input Function

void handleInput(sf::RenderWindow &window)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto *key = event->getIf<sf::Event::KeyPressed>())
        {
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
}

// Score Text

void drawHUD(sf::RenderWindow &window, const sf::Font &font)
{
    sf::Text scoreText(font, "Score: " + std::to_string(score), 24);
    scoreText.setPosition({10.f, 10.f});
    scoreText.setFillColor(sf::Color::White);
    window.draw(scoreText);
}

// Drawing the bricks

void drawBricks(sf::RenderWindow &window)
{
    // Colour for each number in the bricks array (0 = no brick)
    sf::Color colors[4] = {sf::Color(25, 25, 45),
                           sf::Color::Green,
                           sf::Color::Yellow,
                           sf::Color::Red};

    // One shape only, smaller than the box so a gap shows
    sf::RectangleShape brick({BRICK_W - GAP, BRICK_H - GAP});

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
    sf::RectangleShape padddle({PADDLE_W, PADDLE_H});
    padddle.setFillColor(sf::Color::Green);
    padddle.setPosition({paddleX, paddleY});
    window.draw(padddle);
}

// Drawing the Ball

void drawBall(sf::RenderWindow &window)
{
    sf::CircleShape ball({BALL_SIZE / 2});
    ball.setFillColor(sf::Color::White);
    ball.setPosition({ballX, ballY});

    window.draw(ball);
}

// Draw Function

void drawGame(sf::RenderWindow &window, const sf::Font &font)
{
    window.clear(sf::Color(15, 15, 35));
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
