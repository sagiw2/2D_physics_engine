#include "body.h"
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include "timer.h"

float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

float magnitude2d(const sf::Vector2f& v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f normalize(const sf::Vector2f& v)
{
    float length = magnitude2d(v);
    if (length != 0)
        return v / length;
    return sf::Vector2f(0, 0);
}

std::ostream& operator<<(std::ostream &o, const Body &b)
{
    return o << "radius " << b.radius
    << " mass " << b.mass
    << " position x:" << b.position.x << " y: " << b.position.y
    << " velocity x: " << b.velocity.x << " y: " << b.velocity.y
    << " acc x: " << b.acceleration.x << " y: " << b.acceleration.y;
}

bool operator==(const Body &lhs, const Body &rhs)
{
    return lhs.radius == rhs.radius && lhs.mass == rhs.mass &&
    lhs.position == rhs.position && lhs.velocity == rhs.velocity &&
    lhs.acceleration == rhs.acceleration;
}

void Body::addAcceleration(sf::Vector2f force)
{
    this->acceleration = force/this->mass;
}

void Body::addVelocity(float deltaT)
{
    this->velocity += this->acceleration*deltaT;
}

void Body::calcPosition(float deltaT)
{
    this->position += this->velocity*deltaT - this->acceleration*static_cast<float>(deltaT*deltaT*0.5);
}

float Body::calcDistance(Body &otherBdoy)
{
    sf::Vector2f deltaPosition = otherBdoy.getPosition() - this->getPosition();
    return std::hypot(deltaPosition.x, deltaPosition.y);
}

bool Body::collisionDetection(Body & otherBody)
{
    float distance = calcDistance(otherBody);
    if (distance - otherBody.radius - this->radius < 0)
    {
        return true;
    }
    return false;
}

void Body::update(std::vector<Body> &bodies, float deltaT)
{
    // PROFILE_SCOPE("Update Body");
    // gravity forces only for now
    sf::Vector2f totalForce{0.0, 0.0};

    for (auto &otherBody : bodies)
    {
        if (&otherBody != this)
        {
            sf::Vector2f deltaPosition = otherBody.position - this->position;
            float distanceSquared = dot(deltaPosition, deltaPosition);
            float distance = std::sqrt(distanceSquared);

            float force = G*otherBody.mass*this->mass/distanceSquared;
            totalForce += (deltaPosition*force)/distance;
        }
    }
    addAcceleration(totalForce);
    addVelocity(deltaT);
    calcPosition(deltaT);
}

void Body::update(sf::Vector2f force, float deltaT)
{
    addAcceleration(force);
    addVelocity(deltaT);
    calcPosition(deltaT);
}

sf::Vector2f Body::calcForce(sf::Vector2f position, float mass, float deltaT)
{
    // PROFILE_SCOPE("Calculate force on Body by center of mass");
    // gravity forces only for now

    sf::Vector2f deltaPosition = position - this->position;
    float distanceSquared = dot(deltaPosition, deltaPosition);
    float distance = std::sqrt(distanceSquared);

    float force = G*mass*this->mass/distanceSquared;
    return (deltaPosition*force)/distance;
}

void resolveCollision(Body &b1, Body &b2)
{
    // PROFILE_SCOPE("Resolve Collision");
    // std::cout << "B1 with mass " << b1.getMass() << " and vel: " << b1.getVelocity().x << ", " << b1.getVelocity().y << std::endl;
    // std::cout << "B2 with mass " << b2.getMass() << " and vel: " << b2.getVelocity().x << ", " << b2.getVelocity().y << std::endl;

    float collisionCoefficient = 0.9;
    // fix position -> after fix if 2 balls were inside each other, they are now touching
    sf::Vector2f deltaPosition = b2.getPosition() - b1.getPosition();
    sf::Vector2f direction = normalize(b2.getPosition() - b1.getPosition());
    float overlap = b1.calcDistance(b2) - (b1.getRadius() + b2.getRadius());
    b1.setPosition(b1.getPosition() + overlap*direction*b2.getMass()/(b1.getMass() + b2.getMass()));
    b2.setPosition(b2.getPosition() -overlap*direction*b1.getMass()/(b1.getMass() + b2.getMass()));

    // calculate new velocitys
    sf::Vector2f preB1Velocity{b1.getVelocity()};
    deltaPosition = b1.getPosition() - b2.getPosition();
    sf::Vector2f scaledNoraml = (deltaPosition)/(magnitude2d(deltaPosition)*magnitude2d(deltaPosition));
    b1.setVelocity(b1.getVelocity() - collisionCoefficient*(2*b2.getMass()/(b1.getMass() + b2.getMass()))*dot(b1.getVelocity() - b2.getVelocity(), deltaPosition)*(scaledNoraml));

    deltaPosition = b2.getPosition() - b1.getPosition();
    scaledNoraml = (deltaPosition)/(magnitude2d(deltaPosition)*magnitude2d(deltaPosition));
    b2.setVelocity(b2.getVelocity() - collisionCoefficient*(2*b1.getMass()/(b1.getMass() + b2.getMass()))*dot(b2.getVelocity() - preB1Velocity, deltaPosition)*(scaledNoraml));
    // std::cout << "New B1 with mass " << b1.getMass() << " and vel: " << b1.getVelocity().x << ", " << b1.getVelocity().y << std::endl;
    // std::cout << "New B2 with mass " << b2.getMass() << " and vel: " << b2.getVelocity().x << ", " << b2.getVelocity().y << std::endl;
}

Body* getClosestBody(sf::Vector2f &position, std::vector<Body> &bodies)
{
    // PROFILE_SCOPE("Get Closest Body");
    float minDist{MAXFLOAT};
    Body* closest{nullptr};
    for (auto &body : bodies)
    {
        sf::Vector2f deltaPosition = body.getPosition() - position;
        float distance = std::hypot(deltaPosition.x, deltaPosition.y);
        if (distance < minDist && distance < body.getRadius())
        {
            minDist = distance;
            closest = &body;
        }
    }
    return closest;
}
