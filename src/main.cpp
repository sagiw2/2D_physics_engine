#include <SFML/Graphics.hpp>
#include "body.h"

const float TIME_STEP = 1.0f / 60.0f;

void createShapeFromBody(sf::CircleShape &shape, Body &body)
{
    shape.setRadius(body.radius);
    shape.setPosition(body.position.x - body.radius, body.position.y - body.radius);
    shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
}

void addShapesAndBodies(std::vector<sf::CircleShape> &shapes, std::vector<Body> &bodies, const std::vector<Body> &newBodies)
{
    for (Body b : newBodies)
    {
        bodies.emplace_back(b);
        shapes.emplace_back(sf::CircleShape());
        createShapeFromBody(shapes.back(), b);
    }
}

void updatePhysics(std::vector<Body> & bodies, float deltaT)
{
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        bodies[i].update(bodies, deltaT);
        
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            if (bodies[i].collisionDetection(bodies[j]))
            {
                resolveCollision(bodies[i], bodies[j]);
            }
        }
    }
}

void updateShapes(std::vector<Body> &bodies, std::vector<sf::CircleShape> & shapes)
{
    for (size_t i{0}; i < bodies.size(); ++i)
        shapes[i].setPosition(bodies[i].position.x - bodies[i].radius, bodies[i].position.y - bodies[i].radius);
}

sf::VertexArray createArrow(const sf::Vector2f& base, const sf::Vector2f& tip, float wingLength = 10.f, float wingAngleDeg = 30.f)
{
    sf::VertexArray arrow(sf::Lines, 6);

    sf::Vector2f direction = tip - base;
    sf::Vector2f normDirection = normalize(direction);

    arrow[0].position = base;
    arrow[1].position = tip;

    float angleRad = wingAngleDeg * PI / 180.f;

    sf::Vector2f leftWing(
        std::cos(-angleRad) * (-normDirection.x) - std::sin(-angleRad) * (-normDirection.y),
        std::sin(-angleRad) * (-normDirection.x) + std::cos(-angleRad) * (-normDirection.y)
    );

    sf::Vector2f rightWing(
        std::cos(angleRad) * (-normDirection.x) - std::sin(angleRad) * (-normDirection.y),
        std::sin(angleRad) * (-normDirection.x) + std::cos(angleRad) * (-normDirection.y)
    );

    arrow[2].position = tip;
    arrow[3].position = tip + leftWing * wingLength;
    arrow[4].position = tip;
    arrow[5].position = tip + rightWing * wingLength;

    return arrow;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "2D Planetary Physics");
    sf::View view = window.getDefaultView();

    srand(time(NULL));

    Body body1(100, 100, {400.0, 300.0});
    Body body2(10, 1, {10.0, 50.0}, {0.0, 0.0});
    sf::VertexArray arrow;
    
    std::vector<Body> bodies;
    std::vector<sf::CircleShape> shapes;

    addShapesAndBodies(shapes, bodies, {body1, body2});

    sf::Clock clock;
    float accumulatedTime = 0.0f;
    bool gameIsRunning{true};

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
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && gameIsRunning)
            {
                gameIsRunning = false;
                Body newBody(10, 10, window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                arrow = createArrow(newBody.position, window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                addShapesAndBodies(shapes, bodies, {newBody});
            }
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                gameIsRunning = true;
                bodies.back().velocity = sf::Vector2f{arrow[1].position - arrow[0].position};
                arrow.clear();
                clock.restart();
            }
        }

        if (gameIsRunning)
        {
            float deltaTime = clock.restart().asSeconds();
            accumulatedTime += deltaTime;
        
            while (accumulatedTime >= TIME_STEP)
            {
                updatePhysics(bodies, TIME_STEP);
                updateShapes(bodies, shapes);
                accumulatedTime -= TIME_STEP;
            }
        }
        else
        {
            bodies.back().radius += 0.05;
            shapes.back().setRadius(shapes.back().getRadius() + 0.05);
            arrow = createArrow(bodies.back().position, window.mapPixelToCoords(sf::Mouse::getPosition(window)));
            updateShapes(bodies, shapes);
        }
        window.setView(view);
        window.clear();
        for (auto shape : shapes)
            window.draw(shape);
        if (arrow.getVertexCount() != 0)
            window.draw(arrow);
        window.display();
    }

    return 0;
}