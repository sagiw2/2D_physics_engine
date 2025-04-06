#pragma once

#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <math.h>

const float G = 6.674*pow(10, -11)*pow(10, 15);

const float PI = 3.14159265;

struct Body
{
    float radius;
    float mass;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    
    Body(float r, float m, sf::Vector2f pos) : Body(r, m, pos, {0.0, 0.0}, {0.0, 0.0}) {}
    Body(float r, float m, sf::Vector2f pos, sf::Vector2f vel) : Body(r, m, pos, vel, {0.0, 0.0}) {}
    Body(float r, float m, sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f acc) :
    radius{r}, mass{m}, position{pos}, velocity{vel}, acceleration{acc} {}

    friend std::ostream& operator<<(std::ostream &o, const Body &b)
    {
        return o << "radius " << b.radius 
                << " mass " << b.mass 
                << " position x:" << b.position.x << " y: " << b.position.y
                << " velocity x: " << b.velocity.x << " y: " << b.velocity.y 
                << " acc x: " << b.acceleration.x << " y: " << b.acceleration.y;
    }

    friend bool operator==(Body const& lhs, Body const& rhs)
    {
        return lhs.radius == rhs.radius && lhs.mass == rhs.mass &&
                lhs.position == rhs.position && lhs.velocity == rhs.velocity &&
                lhs.acceleration == rhs.acceleration;
    }

    friend float calcDistance(Body &b1, Body &b2);
    
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
    
    bool collisionDetection(Body & otherBody)
    {
        float distance = calcDistance(*this, otherBody);
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

float calcDistance(Body &b1, Body &b2)
{
    sf::Vector2f deltaPosition = b2.position - b1.position;
    return std::hypot(deltaPosition.x, deltaPosition.y);
}

void resolveCollision(Body &b1, Body &b2)
{
    float collisionCoefficient = 0.9;
    // fix position -> after fix if 2 balls were inside each other, they are now touching
    sf::Vector2f deltaPosition = b2.position - b1.position;
    sf::Vector2f direction = normalize(b2.position - b1.position);
    float overlap = calcDistance(b1, b2) - (b1.radius + b2.radius);
    b1.position += overlap*direction*b2.mass/(b1.mass + b2.mass);
    b2.position += -overlap*direction*b1.mass/(b1.mass + b2.mass);
    
    // calculate new velocitys
    sf::Vector2f preB1Velocity{b1.velocity};
    deltaPosition = b1.position - b2.position;
    sf::Vector2f scaledNoraml = (deltaPosition)/static_cast<float>(pow(magnitude2d(deltaPosition), 2));
    b1.velocity -= collisionCoefficient*(2*b2.mass/(b1.mass + b2.mass))*dot(b1.velocity - b2.velocity, deltaPosition)*(scaledNoraml);
    
    deltaPosition = b2.position - b1.position;
    scaledNoraml = (deltaPosition)/static_cast<float>(pow(magnitude2d(deltaPosition), 2));
    b2.velocity -= collisionCoefficient*(2*b1.mass/(b1.mass + b2.mass))*dot(b2.velocity - preB1Velocity, deltaPosition)*(scaledNoraml);
}

Body* getClosestBody(const sf::Vector2f &position, std::vector<Body> &bodies)
{
    float minDist{MAXFLOAT};
    Body* closest{nullptr};
    for (auto &body : bodies)
    {
        sf::Vector2f deltaPosition = body.position - position;
        float distance = std::hypot(deltaPosition.x, deltaPosition.y);
        if (distance < minDist && distance < body.radius)
        {
            minDist = distance;
            closest = &body;
        }
    }
    return closest;
}
