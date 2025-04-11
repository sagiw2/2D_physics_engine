#pragma once

#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <math.h>

const float G = 6.674*pow(10, -11)*pow(10, 15);
const float PI = 3.14159265;


float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

float magnitude2d(const sf::Vector2f& v)
{
    return std::hypot(v.x, v.y);
}

sf::Vector2f normalize(const sf::Vector2f& v)
{
    float length = std::hypot(v.x, v.y);
    if (length != 0)
        return v / length;
    return sf::Vector2f(0, 0);
}

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

    friend std::ostream& operator<<(std::ostream &o, const Body &b)
    {
        return o << "radius " << b.radius 
        << " mass " << b.mass 
        << " position x:" << b.position.x << " y: " << b.position.y
        << " velocity x: " << b.velocity.x << " y: " << b.velocity.y 
        << " acc x: " << b.acceleration.x << " y: " << b.acceleration.y;
    }
    
    friend bool operator==(const Body &lhs, const Body &rhs)
    {
        return lhs.radius == rhs.radius && lhs.mass == rhs.mass &&
        lhs.position == rhs.position && lhs.velocity == rhs.velocity &&
        lhs.acceleration == rhs.acceleration;
    }
    
    void addAcceleration(sf::Vector2f force)
    {
        this->acceleration = force/this->mass;
    }
    
    void addVelocity(float deltaT)
    {
        this->velocity += this->acceleration*deltaT;
    }
    
    void calcPosition(float deltaT)
    {
        this->position += this->velocity*deltaT - this->acceleration*static_cast<float>(deltaT*deltaT*0.5);
    }
    
    float calcDistance(Body &otherBdoy)
    {
        sf::Vector2f deltaPosition = otherBdoy.getPosition() - this->getPosition();
        return std::hypot(deltaPosition.x, deltaPosition.y);
    }

    bool collisionDetection(Body & otherBody)
    {
        float distance = calcDistance(otherBody);
        if (distance - otherBody.radius - this->radius < 0)
        {
            return true;
        }
        return false;
    }

    void update(std::vector<Body> &bodies, float deltaT)
    {
        // gravity forces only for now
        sf::Vector2f totalForce{0.0, 0.0};
        for (auto &otherBody : bodies)
        {
            if (&otherBody != this)
            {
                sf::Vector2f deltaPosition = otherBody.position - this->position;
                float distance = std::hypot(deltaPosition.x, deltaPosition.y);
                
                float force = G*otherBody.mass*this->mass/pow(distance, 2);
                totalForce += (deltaPosition*force)/distance;
            }
        }
        addAcceleration(totalForce);
        addVelocity(deltaT);
        calcPosition(deltaT);
    }
};

void resolveCollision(Body &b1, Body &b2)
{
    std::cout << "B1 with mass " << b1.getMass() << " and vel: " << b1.getVelocity().x << ", " << b1.getVelocity().y << std::endl;
    std::cout << "B2 with mass " << b1.getMass() << " and vel: " << b2.getVelocity().x << ", " << b2.getVelocity().y << std::endl;

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
    sf::Vector2f scaledNoraml = (deltaPosition)/static_cast<float>(pow(magnitude2d(deltaPosition), 2));
    b1.setVelocity(b1.getVelocity() - collisionCoefficient*(2*b2.getMass()/(b1.getMass() + b2.getMass()))*dot(b1.getVelocity() - b2.getVelocity(), deltaPosition)*(scaledNoraml));
    
    deltaPosition = b2.getPosition() - b1.getPosition();
    scaledNoraml = (deltaPosition)/static_cast<float>(pow(magnitude2d(deltaPosition), 2));
    b2.setVelocity(b2.getVelocity() - collisionCoefficient*(2*b1.getMass()/(b1.getMass() + b2.getMass()))*dot(b2.getVelocity() - preB1Velocity, deltaPosition)*(scaledNoraml));
}

Body* getClosestBody(sf::Vector2f &position, std::vector<Body> &bodies)
{
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
