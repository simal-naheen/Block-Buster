#include <SFML/Graphics.hpp>
#include <optional>
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

    while (window.isOpen())
    {
        handleInput(window);
        updateGame();
        drawGame(window);
    }
}