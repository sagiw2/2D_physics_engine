#pragma once

#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <math.h>

const float G = 6.674*pow(10, -11)*pow(10, 15);

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
    
    // void collisionDetection(std::vector<Body> & bodies)
    // {
    //     for (auto &otherBody : bodies)
    //     {
    //         if (&otherBody != this)
    //         {
    //             float distance = calcDistance(*this, otherBody);
    //             if (distance < otherBody.radius + this->radius)
    //             {
    //                 std::cout << "Coliding" << std::endl;
    //             }
    //         }
    //     }
    // }

    
    void update(std::vector<Body> &bodies, float deltaT)
    {
        // gravity forces only for now
        sf::Vector2f totalForce{0.0, 0.0};
        for (auto &otherBody : bodies)
        {
            if (&otherBody != this)
            {
                sf::Vector2f direction = otherBody.position - this->position;
                float distance = std::hypot(direction.x, direction.y);
                if (distance < otherBody.radius)
                distance = otherBody.radius;
                
                if (distance > 0.01)
                {
                    float force = G*otherBody.mass*this->mass/pow(distance, 2);
                    totalForce += (direction*force)/distance;
                }
            }
        }
        addAcceleration(totalForce);
        addVelocity(deltaT);
        calcPosition(deltaT);
        // collisionDetection(bodies);
    }
};

float calcDistance(Body &b1, Body &b2)
{
    sf::Vector2f deltaPosition = b2.position - b1.position;
    return std::hypot(deltaPosition.x, deltaPosition.y);
}

void resolveCollision(Body &b1, Body &b2)
{
    float distance = calcDistance(b1, b2);
    float overlap = b1.radius + b2.radius - distance;

    if (overlap > 0)
    {
        sf::Vector2f direction = b2.position - b1.position;
        direction = direction/sqrt(direction.x*direction.x + direction.y*direction.y);

        float padding = 1.0f;
        b1.position += direction * padding * (overlap * (b2.mass / (b1.mass + b2.mass)));
        b2.position -= direction * padding * (overlap * (b1.mass / (b1.mass + b2.mass)));

        sf::Vector2f relativeVelocity = b2.velocity - b1.velocity;
        
        float velocityAlongNormal = relativeVelocity.x*direction.x + relativeVelocity.y*direction.y;
        
        if (velocityAlongNormal < 0)
        {
            float coff = 0.8f; // 1 is the coefficient for elastic collision
            float impulse = -(1 + coff) * velocityAlongNormal;
            impulse /= (1 / b1.mass + 1 / b2.mass);

            sf::Vector2f impulseVec = direction * impulse;
            b1.velocity -= impulseVec / b1.mass;
            b2.velocity += impulseVec / b2.mass;

        }
    }
}

