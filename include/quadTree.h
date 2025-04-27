#include <memory>
#include <vector>

class Body;

struct Rect
{
    float x, y, width, height;

    Rect(const float x, const float y, const float width, const float height): x(x), y(y), width(width), height(height) {}

    bool contains(const Body& body) const;

    bool intersects(const Rect& range) const;
};

class QuadTree
{

private:

    static const int CAPACITY = 1;
    Rect boundary;
    std::vector<Body> bodies;
    bool divided = false;

    float totalMass{0};
    sf::Vector2f centerOfMass{0.0, 0.0};

    std::unique_ptr<QuadTree> topLeft;
    std::unique_ptr<QuadTree> topRight;
    std::unique_ptr<QuadTree> bottomLeft;
    std::unique_ptr<QuadTree> bottomRight;

    void subdivide();

public:

    QuadTree(const Rect& boundary) : boundary(boundary) {}

    bool insert(const Body& body);

    void query(const Rect& range, std::vector<Body>& found) const;

    std::vector<sf::RectangleShape> show() const;

    sf::Vector2f calculateForces(Body& body, float deltaT);
};

