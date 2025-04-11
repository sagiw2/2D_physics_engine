#pragma once

#include <SFML/Graphics.hpp>

class viewHandler
{
private:
    sf::View view;
    float moveViewDistance = 10.f;

public:
    viewHandler(const sf::View &v): view(v) {}

    void viewMoveUp() { view.move(0, -moveViewDistance); }
    void viewMoveDown() { view.move(0, moveViewDistance); }
    void viewMoveLeft() { view.move(-moveViewDistance, 0); }
    void viewMoveRight() { view.move(moveViewDistance, 0); }
    void zoomView(float scrollWheelDelta)
    {
        float zoomFactor = (scrollWheelDelta > 0) ? 0.9f : 1.1f;
        view.zoom(zoomFactor);
    }

    sf::View getView() {return view;}
};
