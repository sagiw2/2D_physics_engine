#include <SFML/Graphics.hpp>
#include "viewHandler.h"
#include "body.h"
#include "timer.h"

#include "quadTree.h"

const float TIME_STEP = 1.0f / 60.0f;
int counter = 0;

void createShapeFromBody(sf::CircleShape &shape, Body &body)
{
    shape.setRadius(body.getRadius());
    shape.setPosition(body.getPosition().x, body.getPosition().y);
    shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
}

void addShapesAndBodies(std::vector<sf::CircleShape> &shapes, std::vector<Body> &bodies, const std::vector<Body> &newBodies)
{
    // PROFILE_SCOPE("add Shapes and Bodies");
    for (Body b : newBodies)
    {
        bodies.emplace_back(b);
        shapes.emplace_back(sf::CircleShape());
        createShapeFromBody(shapes.back(), b);
    }
}

void updatePhysics(std::vector<Body> & bodies, float deltaT)
{
    // PROFILE_SCOPE("Update Physics");
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
    // PROFILE_SCOPE("Update shapes");
    for (size_t i{0}; i < bodies.size(); ++i)
        shapes[i].setPosition(bodies[i].getPosition().x - bodies[i].getRadius(), bodies[i].getPosition().y - bodies[i].getRadius());
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

void nonUserCreateNewBody(sf::RenderWindow &window, std::vector<sf::CircleShape> &shapes, sf::VertexArray &arrow, std::vector<Body> &bodies)
{
    // PROFILE_SCOPE("non User Create New Body");
    Body newBody(10, 10, window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    arrow = createArrow(newBody.getPosition(), window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    addShapesAndBodies(shapes, bodies, {newBody});
}

void createNewBody(sf::RenderWindow &window, bool &gameIsRunning, std::vector<sf::CircleShape> &shapes, sf::VertexArray &arrow, std::vector<Body> &bodies)
{
    gameIsRunning = false;
    Body newBody(10, 10, window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    arrow = createArrow(newBody.getPosition(), window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    addShapesAndBodies(shapes, bodies, {newBody});
}

void unPauseAfterCreationOfNewBody(bool &gameIsRunning, std::vector<Body> &bodies, sf::VertexArray &arrow, sf::Clock &clock)
{
    gameIsRunning = true;
    bodies.back().setVelocity(sf::Vector2f{arrow[1].position - arrow[0].position});
    arrow.clear();
    clock.restart();
}

void updateWindow(sf::RenderWindow &window, viewHandler &view, std::vector<sf::CircleShape> &shapes, std::vector<sf::RectangleShape> &quads, sf::VertexArray &arrow)
{
    window.setView(view.getView());
    window.clear();
    for (auto &shape : shapes)
        window.draw(shape);
    for (auto &quad : quads)
        window.draw(quad);
    if (arrow.getVertexCount() != 0)
        window.draw(arrow);
    window.display();
}

sf::CircleShape* getClosestShape(const sf::Vector2f &position, std::vector<sf::CircleShape> &shapes)
{
    // PROFILE_SCOPE("Get Closest Shape");
    float minDist{MAXFLOAT};
    sf::CircleShape* closest{nullptr};
    for (auto &shape : shapes)
    {
        sf::Vector2f deltaPosition = shape.getPosition() - position;
        float distance = std::hypot(deltaPosition.x, deltaPosition.y);
        if (distance < minDist && distance < shape.getRadius())
        {
            minDist = distance;
            closest = &shape;
        }
    }
    return closest;
}

void removeBodyAndShape(sf::RenderWindow &window, std::vector<Body> &bodies, std::vector<sf::CircleShape> &shapes)
{
    sf::Vector2i mousePositionWorld = sf::Mouse::getPosition(window);
    sf::Vector2f mousePositionWindow = window.mapPixelToCoords(mousePositionWorld);
    if (Body* closestBody = getClosestBody(mousePositionWindow, bodies))
    {
        auto it = std::find(bodies.begin(), bodies.end(), *closestBody);
        if (it != bodies.end())
        {
            auto index = std::distance(bodies.begin(), it);

            shapes.erase(shapes.begin() + index);
            bodies.erase(it);
        }
    }
}

void updateCreatedShape(sf::RenderWindow &window, std::vector<Body> &bodies, std::vector<sf::CircleShape> &shapes, sf::VertexArray &arrow)
{
    bodies.back().setRadius(bodies.back().getRadius() + 0.05);
    bodies.back().setMass(bodies.back().getRadius());
    shapes.back().setRadius(shapes.back().getRadius() + 0.05);
    arrow = createArrow(bodies.back().getPosition(), window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    updateShapes(bodies, shapes);
}

// int main()
// {
//     sf::RenderWindow window(sf::VideoMode(800, 600), "2D Planetary Physics");

//     viewHandler viewHandler(window.getDefaultView());

//     srand(time(NULL));


//     Body body1(100, 100, {400.0, 300.0});
//     Body body2(10, 10, {10.0, 50.0}, {0.0, 0.0});
//     counter += 2;
//     sf::VertexArray arrow;

//     std::vector<Body> bodies;
//     std::vector<sf::CircleShape> shapes;
//     bodies.reserve(1000);
//     shapes.reserve(1000);

//     addShapesAndBodies(shapes, bodies, {body1, body2});

//     sf::Clock clock;
//     float accumulatedTime = 0.0f;
//     bool gameIsRunning{true};

//     while (window.isOpen())
//     {
//         sf::Event event;
//         while (window.pollEvent(event))
//         {
//             if (event.type == sf::Event::Closed)
//                 window.close();
//             if (event.type == sf::Event::MouseWheelScrolled) viewHandler.zoomView(event.mouseWheelScroll.delta);
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) viewHandler.viewMoveUp();
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) viewHandler.viewMoveDown();
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) viewHandler.viewMoveLeft();
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) viewHandler.viewMoveRight();
//             if(event.type == sf::Event::MouseButtonPressed &&
//                 event.mouseButton.button == sf::Mouse::Left)
//             {
//                 if (gameIsRunning)
//                     createNewBody(window, gameIsRunning, shapes, arrow, bodies);
//                 else
//                 {
//                     unPauseAfterCreationOfNewBody(gameIsRunning, bodies, arrow, clock);
//                 }
//             }
//             if(event.type == sf::Event::MouseButtonPressed &&
//                 event.mouseButton.button == sf::Mouse::Middle)
//                     removeBodyAndShape(window, bodies, shapes);

//             // for debug
//             // if(event.type == sf::Event::MouseButtonPressed &&
//             //     event.mouseButton.button == sf::Mouse::Right)
//             // {
//             //     sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//             //     if (auto closestBody = getClosestBody(mousePosition, bodies))
//             //     {
//             //         std::cout << *closestBody << std::endl;
//             //     }
//             // }
//         }

//         if (gameIsRunning)
//         {
//             // need to read on frame independant calculation
//             float deltaTime = clock.restart().asSeconds();
//             accumulatedTime += deltaTime;

//             while (accumulatedTime >= TIME_STEP)
//             {
//                 nonUserCreateNewBody(window, shapes, arrow, bodies);
//                 // std::cout << "Number of bodies: " << ++counter << std::endl;
//                 updatePhysics(bodies, TIME_STEP);
//                 updateShapes(bodies, shapes);
//                 accumulatedTime -= TIME_STEP;
//             }
//         }
//         else
//             updateCreatedShape(window, bodies, shapes, arrow);

//         updateWindow(window, viewHandler, shapes, arrow);
//     }

//     return 0;
// }

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "2D Planetary Physics");

    viewHandler viewHandler(window.getDefaultView());

    srand(time(NULL));


    Body body1(100, 100, {410.0, 300.0});
    Body body2(10, 10, {10.0, 50.0}, {0.0, 0.0});
    sf::VertexArray arrow;

    std::vector<Body> bodies;
    std::vector<sf::CircleShape> shapes;
    std::vector<sf::RectangleShape> quads;
    bodies.reserve(1000);
    shapes.reserve(1000);

    addShapesAndBodies(shapes, bodies, {body1, body2});

    sf::Clock clock;
    float accumulatedTime = 0.0f;
    bool gameIsRunning{true};

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseWheelScrolled) viewHandler.zoomView(event.mouseWheelScroll.delta);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) viewHandler.viewMoveUp();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) viewHandler.viewMoveDown();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) viewHandler.viewMoveLeft();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) viewHandler.viewMoveRight();
            if(event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                if (gameIsRunning)
                    createNewBody(window, gameIsRunning, shapes, arrow, bodies);
                else
                {
                    unPauseAfterCreationOfNewBody(gameIsRunning, bodies, arrow, clock);
                }
            }
            if(event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Middle)
                    removeBodyAndShape(window, bodies, shapes);
        }

        if (gameIsRunning)
        {
            // need to read on frame independant calculation
            float deltaTime = clock.restart().asSeconds();
            accumulatedTime += deltaTime;

            while (accumulatedTime >= TIME_STEP)
            {
                QuadTree qTree{Rect{viewHandler.getView().getCenter().x, viewHandler.getView().getCenter().y, viewHandler.getView().getSize().x, viewHandler.getView().getSize().y}};
                for (auto& body : bodies)
                    qTree.insert(body);
                for (auto& body : bodies)
                {
                    sf::Vector2f totalForces = qTree.calculateForces(body, TIME_STEP);
                    body.update(totalForces, deltaTime);
                }
                // updatePhysics(bodies, TIME_STEP);
                quads.clear();
                quads = qTree.show();
                updateShapes(bodies, shapes);
                accumulatedTime -= TIME_STEP;
            }
        }
        else
            updateCreatedShape(window, bodies, shapes, arrow);

        updateWindow(window, viewHandler, shapes, quads, arrow);
    }

    return 0;
}