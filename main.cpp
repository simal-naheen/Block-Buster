#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
using namespace sf;
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

void brickArr(int arr[][100], int &rows, int &cols)
{
    std::ifstream fin("level.txt");

    // Checking the file

    if (!fin)
    {
        std::cout << "File Not Found";
        return;
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
}

void drawGame(RenderWindow &window)
{

    window.clear(sf::Color(15, 15, 35));
    window.display();
}

// Main Body
int main()
{
    sf::RenderWindow window(sf::VideoMode({600, 700}), "Block Buster");
    window.setFramerateLimit(60);
    int bricks[100][100];
    int rows, cols;
    brickArr(bricks, rows, cols);
    std::cout << rows << cols;

    while (window.isOpen())
    {
        handleInput(window);
        updateGame();
        drawGame(window);
    }
}
