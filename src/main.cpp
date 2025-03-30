#include <SFML/Graphics.hpp>
#include "body.h"

const float TIME_STEP = 1.0f / 60.0f;

void createShapeFromBody(sf::CircleShape &shape, Body &body)
{
    shape.setRadius(body.radius);
    shape.setPosition(body.position.x - body.radius, body.position.y - body.radius);
    shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
}

void updatePhysics(std::vector<Body> & bodies, float deltaT)
{
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        bodies[i].update(bodies, deltaT);
        
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            resolveCollision(bodies[i], bodies[j]);
        }
    }
}

void updateShapes(std::vector<Body> &bodies, std::vector<sf::CircleShape> & shapes)
{
    
    for (size_t i{0}; i < bodies.size(); ++i)
    {
        shapes[i].setPosition(bodies[i].position.x - bodies[i].radius, bodies[i].position.y - bodies[i].radius);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "2D Physics Engine");
    sf::View view = window.getDefaultView();

    srand(time(NULL));

    Body body1(100, 100, {400.0, 300.0});
    Body body2(10, 1, {10.0, 50.0}, {0.0, 0.0});
    std::vector<Body> bodies{body1, body2};

    std::vector<sf::CircleShape> shapes;
    for (Body b : bodies)
    {
        shapes.emplace_back(sf::CircleShape());
        createShapeFromBody(shapes.back(), b);
    }

    sf::Clock clock;
    float accumulatedTime = 0.0f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                float zoomFactor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
                view.zoom(zoomFactor);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) view.move(0, -10);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) view.move(0, 10);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) view.move(-10, 0);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) view.move(10, 0);
        }

        float deltaTime = clock.restart().asSeconds();
        accumulatedTime += deltaTime;
    
        while (accumulatedTime >= TIME_STEP)
        {
            updatePhysics(bodies, TIME_STEP);
            updateShapes(bodies, shapes);
            accumulatedTime -= TIME_STEP;
        }

        window.setView(view);
        window.clear();
        for (auto shape : shapes)
            window.draw(shape);
        window.display();
    }

    return 0;
}