#include "body.h"
#include "quadTree.h"


bool Rect::contains(const Body& body) const
{
    sf::Vector2f point = body.getPosition();
    return point.x >= x - width/2 && point.x <= x + width/2 &&
            point.y >= y - height/2 && point.y <= y + height/2;
}

bool Rect::intersects(const Rect& range) const
{
    return !(range.x > x + width || range.x + range.width < x ||
            range.y > y + height || range.y + range.height < y);
}

void QuadTree::subdivide()
{
    float x = boundary.x;
    float y = boundary.y;
    float w = boundary.width / 2;
    float h = boundary.height / 2;

    topLeft = std::make_unique<QuadTree>(Rect{x - w/2, y - h/2, w, h});
    topRight = std::make_unique<QuadTree>(Rect{x + w/2, y - h/2, w, h});
    bottomLeft = std::make_unique<QuadTree>(Rect{x - w/2, y + h/2, w, h});
    bottomRight = std::make_unique<QuadTree>(Rect{x + w/2, y + h/2, w, h});

    divided = true;
}

bool QuadTree::insert(const Body& body)
{
    if (!boundary.contains(body))
        return false;

    centerOfMass = (totalMass*centerOfMass + body.getMass()*body.getPosition())/(totalMass + body.getMass());
    totalMass += body.getMass();

    if (!divided && bodies.size() < CAPACITY)
    {
        bodies.push_back(body);
        return true;
    }

    if (!divided)
    {
        subdivide();
        insert(bodies[0]);
        bodies.pop_back();
    }

    if (topLeft->insert(body)) return true;
    if (topRight->insert(body)) return true;
    if (bottomLeft->insert(body)) return true;
    if (bottomRight->insert(body)) return true;

    return false;
}

void QuadTree::query(const Rect& range, std::vector<Body>& found) const
{
    if (!boundary.intersects(range))
        return;

    for (const auto& body : bodies)
    {
        if (range.contains(body))
            found.push_back(body);
    }

    if (divided)
    {
        topLeft->query(range, found);
        topRight->query(range, found);
        bottomLeft->query(range, found);
        bottomRight->query(range, found);
    }
}

std::vector<sf::RectangleShape> QuadTree::show() const
{
    std::vector<sf::RectangleShape> bbs;
    sf::RectangleShape b(sf::Vector2f{boundary.width, boundary.height});
    b.setPosition(boundary.x - boundary.width/2, boundary.y - boundary.height/2);
    b.setFillColor(sf::Color::Transparent);
    b.setOutlineColor(sf::Color::White);
    b.setOutlineThickness(5);
    bbs.emplace_back(b);
    if (divided)
    {
        auto topLeftBBs = topLeft->show();
        bbs.insert(bbs.end(), topLeftBBs.begin(), topLeftBBs.end());

        auto topRightBBs = topRight->show();
        bbs.insert(bbs.end(), topRightBBs.begin(), topRightBBs.end());

        auto bottomLeftBBs = bottomLeft->show();
        bbs.insert(bbs.end(), bottomLeftBBs.begin(), bottomLeftBBs.end());

        auto bottomRightBBs = bottomRight->show();
        bbs.insert(bbs.end(), bottomRightBBs.begin(), bottomRightBBs.end());
    }
    return bbs;
}

sf::Vector2f QuadTree::calculateForces(Body &body, float deltaT)
{
    sf::Vector2f totalForces{0.0, 0.0};

    if (!boundary.contains(body))
        return body.calcForce(centerOfMass, totalMass, deltaT);

    if (divided)
    {
        totalForces += topLeft->calculateForces(body, deltaT);
        totalForces += topRight->calculateForces(body, deltaT);
        totalForces += bottomLeft->calculateForces(body, deltaT);
        totalForces += bottomRight->calculateForces(body, deltaT);
    }

    return totalForces;
}
