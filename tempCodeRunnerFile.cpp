#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
using namespace sf;

// Creating the Variables and constants
int bricks[8][10];
int rows, cols;
int lives = 3;
int score = 100;
// Constants

// Brick
int const brickW = 60;
int const brickH = 25;
// Paddle
int const paddleW = 100;
int const paddleH = 14;
// Ball
int const ballW = 12;
int const ballH = 12;

// Input Function

void handleInput(RenderWindow &window)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

void updateGame()
{
}

// Writing getting the values in 2D Array

bool brickArr(int arr[][10], int &rows, int &cols)
{
    std::ifstream fin("level.txt");

    // Checking the file

    if (!fin)
    {
        std::cout << "File Not Found";
        return false;
    }

    // Checking the rows and cols

    fin >> rows >> cols;

    // Inputting 2D array

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fin >> arr[i][j];
        }
    }
    return true;
}

void scoreText(RenderWindow &window, Font &font)
{

    Text scoreText(font, "Score : 0", 40);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setPosition({100.f, 100.f});
    scoreText.setFillColor(Color::White);
    window.draw(scoreText);
}

void displayText(RenderWindow &window)
{
    Font font;

    if (!font.openFromFile("arial.ttf"))
    {
        std::cout << "Font not Loaded! ";
        return;
    }
    scoreText(window, font);
}

void drawGame(RenderWindow &window)
{

    // Creating Bricks Array

    window.clear(sf::Color(15, 15, 35));
    displayText(window);
    window.display();
}

// Main Body
int main()
{

    // Window Creation
    sf::RenderWindow window(sf::VideoMode({600, 700}), "Block Buster");
    window.setFramerateLimit(60);
    bool fileOpened = brickArr(bricks, rows, cols);
    if (!fileOpened)
    {
        return -1;
    }
    while (window.isOpen())
    {
        handleInput(window);
        updateGame();
        drawGame(window);
    }
}
