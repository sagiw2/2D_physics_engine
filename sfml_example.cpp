#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;  // Create an event object
        while (window.pollEvent(event))  // Poll the event
        {
            if (event.type == sf::Event::Closed)  // Check if the window was closed
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}