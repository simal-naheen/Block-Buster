#include <SFML/Graphics.hpp>
#include <optional>

// Main Body
int main()
{
    sf::RenderWindow window(sf::VideoMode({600, 700}), "Block Buster");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(15, 15, 35));

        window.display();
    }
}