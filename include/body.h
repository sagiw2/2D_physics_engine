#pragma once

#include <math.h>
#include <SFML/Graphics.hpp>

const float G = 6.674*pow(10, -11)*pow(10, 15);
const float PI = 3.14159265;


float dot(const sf::Vector2f& a, const sf::Vector2f& b);

float magnitude2d(const sf::Vector2f& v);

sf::Vector2f normalize(const sf::Vector2f& v);

class Body
{
private:
    float radius;
    float mass;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;

public:
    Body(float r, float m, sf::Vector2f pos)
        : Body(r, m, pos, {0.0, 0.0}, {0.0, 0.0}) {}
    Body(float r, float m, sf::Vector2f pos, sf::Vector2f vel)
        : Body(r, m, pos, vel, {0.0, 0.0}) {}
    Body(float r, float m, sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f acc)
        : radius{r}, mass{m}, position{pos}, velocity{vel}, acceleration{acc} {}

    float getRadius() const { return radius; }
    float getMass() const { return mass; }
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getVelocity() const { return velocity; }
    sf::Vector2f getAcceleration() const { return acceleration; }

    void setMass(const float m) { mass = m; }
    void setRadius(const float rad) { radius = rad; }
    void setPosition(const sf::Vector2f& pos) { position = pos; }
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }

    friend std::ostream& operator<<(std::ostream &o, const Body &b);

    friend bool operator==(const Body &lhs, const Body &rhs);

    void addAcceleration(sf::Vector2f force);

    void addVelocity(float deltaT);

    void calcPosition(float deltaT);

    float calcDistance(Body &otherBdoy);

    bool collisionDetection(Body & otherBody);

    void update(std::vector<Body> &bodies, float deltaT);

    void update(sf::Vector2f force, float deltaT);

    sf::Vector2f calcForce(sf::Vector2f position, float mass, float deltaT);
};

void resolveCollision(Body &b1, Body &b2);

Body* getClosestBody(sf::Vector2f &position, std::vector<Body> &bodies);
