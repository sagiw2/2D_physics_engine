#pragma once

#include <SFML/Graphics.hpp>

class viewHandler
{
private:
    sf::View view;
    sf::Vector2f baseSize;
    float moveViewDistance = 10.f;
    float zoomLevel = 1.0f;
    const float minZoom = 0.1f;
    const float maxZoom = 10.0f;

public:
    viewHandler(const sf::View &v): view(v), baseSize(v.getSize()) {}

    void viewMoveUp() { view.move(0, -moveViewDistance); }
    void viewMoveDown() { view.move(0, moveViewDistance); }
    void viewMoveLeft() { view.move(-moveViewDistance, 0); }
    void viewMoveRight() { view.move(moveViewDistance, 0); }
    void zoomView(float scrollWheelDelta)
    {
        float zoomFactor = (scrollWheelDelta > 0) ? 0.9f : 1.1f;
        zoomLevel *= zoomFactor;
        zoomLevel = std::clamp(zoomLevel, minZoom, maxZoom);
        view.setSize(baseSize);
        view.zoom(zoomLevel);
    }

    sf::View getView() {return view;}
};
