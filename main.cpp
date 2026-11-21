#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
#include <string>

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

// Paddle
const int PADDLE_W = 100;
const int PADDLE_H = 14;

// Ball
const int BALL_SIZE = 12;

// Creating the Variables

int bricks[ROWS][COLS];
int lives = 3;
int score = 0;

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

// Input Function

void handleInput(sf::RenderWindow &window)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

// Update Function

void updateGame()
{
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
}

// Draw Function

void drawGame(sf::RenderWindow &window, const sf::Font &font)
{
    window.clear(sf::Color(15, 15, 35));
    drawBricks(window);
    drawHUD(window, font);
    window.display();
}

// Main Body
int main()
{

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